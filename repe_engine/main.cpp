// main.cpp
#include "core/EventQueue.h"
#include "core/EventProcessor.h"
#include "input_sources/FileWatcher.h"
#include "input_sources/SocketListener.h"
#include "input_sources/RestApiEndpoint.h"
#include "input_sources/TimerScheduler.h"
#include "rules/RuleManager.h"
#include "action_dispatcher/ActionDispatcher.h"
#include "actions/ActionFactory.h" // To call registerAllDefaultActions()

#include <iostream>  // For standard input/output
#include <fstream>   // For creating dummy data files
#include <thread>    // For std::thread
#include <chrono>    // For std::chrono
#include <atomic>    // For atomic flag to stop RuleEngine thread
#include <limits>    // For std::numeric_limits

// Declare the function to register all default actions.
// This function is defined in actions/ActionFactory.cpp.
extern void registerAllDefaultActions();

// Atomic flag to control the main loop of the RuleEngine thread.
// It allows for graceful shutdown of the thread.
std::atomic<bool> rule_engine_running(true);

// Function to run the Rule Engine in a separate thread.
// It continuously pops events from the EventQueue, evaluates them against rules,
// and dispatches actions if rules match.
// @param event_queue: Reference to the EventQueue from which to retrieve events.
// @param action_dispatcher: Reference to the ActionDispatcher to dispatch triggered actions.
void runRuleEngine(EventQueue& event_queue, ActionDispatcher& action_dispatcher) {
    while (rule_engine_running) { // Loop as long as the rule_engine_running flag is true.
        std::cout << "\n[RuleEngine Thread] Waiting for event from queue..." << std::endl;
        // Use tryPop() to avoid blocking indefinitely when shutting down.
        // If the queue is empty, it returns std::nullopt.
        std::optional<Event> opt_event = event_queue.tryPop();

        if (opt_event) { // Check if an event was successfully retrieved.
            Event event = std::move(opt_event.value()); // Move the event out of optional.
            std::cout << "[RuleEngine Thread] Received event:\n" << event.toString() << std::endl;

            // Evaluate the event against all loaded rules using the RuleManager.
            std::vector<nlohmann::json> triggered_actions = RuleManager::getInstance().evaluate(event);

            // Dispatch any actions that were triggered by matching rules.
            if (!triggered_actions.empty()) {
                std::cout << "[RuleEngine Thread] Dispatching " << triggered_actions.size() << " actions for event ID: " << event.id << std::endl;
                action_dispatcher.dispatch(triggered_actions, event);
            } else {
                std::cout << "[RuleEngine Thread] No rules matched for event ID: " << event.id << std::endl;
            }
        } else {
            // If the queue is empty, pause briefly to prevent a busy-wait loop.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    std::cout << "[RuleEngine Thread] Stopped." << std::endl;
}

int main() {
    // 1. Register all available Action types with the ActionFactory.
    // This ensures the factory knows how to create instances of LogAction, HttpAction, etc.
    registerAllDefaultActions();

    // 2. Initialize core components of the REPE system.
    EventQueue event_queue;             // The thread-safe queue for events.
    EventProcessor event_processor(event_queue); // Processes raw data into Events and pushes to queue.
    ActionDispatcher action_dispatcher; // Dispatches actions to be executed.

    // 3. Prepare a dummy rules configuration file (rules.json).
    // This file defines the rules that the RuleEngine will load and evaluate.
    std::ofstream rules_file("rules.json");
    if (rules_file.is_open()) {
        rules_file << R"(
        [
            {
                "id": "high_temp_alert",
                "condition": {
                    "and": [
                        {"key": "temperature", "operator": ">", "value": 30.0},
                        {"key": "location", "operator": "==", "value": "LivingRoom"}
                    ]
                },
                "actions": [
                    {"type": "log", "message": "ALERT: High temperature ({temperature}C) detected in {location}!"},
                    {"type": "http", "url": "http://localhost:8080/alerts", "method": "POST", "payload": {"alert_type": "high_temp", "temp": "{temperature}", "loc": "{location}"}}
                ]
            },
            {
                "id": "system_status_ok",
                "condition": {
                    "key": "status", "operator": "==", "value": "OK"
                },
                "actions": [
                    {"type": "log", "message": "System status is OK. Source: {source}."}
                ]
            },
            {
                "id": "low_humidity_warning",
                "condition": {
                    "and": [
                        {"key": "humidity", "operator": "<", "value": 40},
                        {"not": {"key": "alert_sent", "operator": "==", "value": true}}
                    ]
                },
                "actions": [
                    {"type": "log", "message": "WARNING: Low humidity ({humidity}%) detected. Consider humidifying."}
                ]
            }
        ]
        )";
        rules_file.close();
        std::cout << "Created dummy rules.json" << std::endl;
    } else {
        std::cerr << "ERROR: Could not create rules.json file. Exiting." << std::endl;
        return 1; // Exit if rules file cannot be created.
    }

    // 4. Load the rules into the RuleManager.
    // This makes the rules available for evaluation by the RuleEngine thread.
    try {
        RuleManager::getInstance().loadRules("rules.json");
    } catch (const std::exception& e) {
        std::cerr << "ERROR loading rules: " << e.what() << ". Exiting." << std::endl;
        return 1; // Exit if rules cannot be loaded.
    }

    // 5. Initialize and start various InputSources.
    // These sources will generate raw data and send it to the EventProcessor.

    // Create dummy file for FileWatcher to monitor.
    std::ofstream dummy_json_file("sensor_data.json");
    dummy_json_file << R"({"type": "sensor_reading", "temperature": 25.0, "humidity": 60, "location": "LivingRoom"})";
    dummy_json_file.close();

    std::ofstream dummy_status_file("system_status.txt");
    dummy_status_file << "STATUS:OK";
    dummy_status_file.close();

    FileWatcher json_file_watcher("sensor_data.json", "json_string");
    json_file_watcher.start(event_processor);

    FileWatcher text_file_watcher("system_status.txt", "string_value"); // Assumes the entire file content is a single string value.
    text_file_watcher.start(event_processor);

    SocketListener socket_listener(12345); // Skeleton: Simulates receiving data on port 12345.
    socket_listener.start(event_processor);

    RestApiEndpoint rest_api_endpoint(8080); // Skeleton: Simulates receiving API requests on port 8080.
    rest_api_endpoint.start(event_processor);

    TimerScheduler timer_scheduler(std::chrono::seconds(7), "heartbeat", "System heartbeat check.");
    timer_scheduler.start(event_processor);

    // 6. Run the Rule Engine in a separate thread.
    // This thread will continuously process events from the queue.
    std::thread rule_engine_worker(runRuleEngine, std::ref(event_queue), std::ref(action_dispatcher));

    // 7. Main application loop (waits for a stop command).
    std::cout << "\nREPE system running. Enter 'temp' to input temperature, 'status' for system status, or 'q' to quit..." << std::endl;
    std::string command;
    while (std::cin >> command && command != "q") {
        if (command == "temp") {
            double temp_val;
            std::cout << "Enter temperature: ";
            if (std::cin >> temp_val) {
                // Manually push a temperature event.
                event_processor.processRawData("manual_temp_input", "temperature_reading", temp_val, "temperature");
                // Also push location info to allow the rule to match.
                event_processor.processRawData("manual_temp_input", "location_info", "LivingRoom", "location");
            } else {
                std::cout << "Invalid input. Please enter a number." << std::endl;
                std::cin.clear(); // Clear error flags
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            }
        } else if (command == "status") {
            std::string status_val;
            std::cout << "Enter status (OK/ERROR): ";
            std::cin >> status_val;
            // Manually push a system status event.
            event_processor.processRawData("manual_status_input", "system_status", status_val, "status");
        } else {
            std::cout << "Unknown command. Enter 'temp', 'status', or 'q'." << std::endl;
        }
    }

    // 8. Graceful shutdown of the system.
    std::cout << "Stopping REPE system..." << std::endl;
    rule_engine_running = false; // Signal the RuleEngine thread to stop.

    // Stop all InputSources.
    json_file_watcher.stop();
    text_file_watcher.stop();
    socket_listener.stop();
    rest_api_endpoint.stop();
    timer_scheduler.stop();

    // Ensure the RuleEngine thread finishes its execution.
    if (rule_engine_worker.joinable()) {
        rule_engine_worker.join();
    }

    std::cout << "REPE system stopped." << std::endl;

    return 0;
}

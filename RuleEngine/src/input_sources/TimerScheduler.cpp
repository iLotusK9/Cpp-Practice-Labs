// input_sources/TimerScheduler.cpp
#include "TimerScheduler.h"
#include "core/EventProcessor.h" // Cần include EventProcessor để sử dụng nó
#include <chrono> // For std::chrono::seconds, std::this_thread::sleep_for
#include <iostream> // For std::cout

// Constructor for TimerScheduler.
// @param interval: The duration between each event trigger.
// @param event_type: The type string for the generated event (e.g., "heartbeat").
// @param message: A message string to be included in the generated event.
TimerScheduler::TimerScheduler(std::chrono::seconds interval, const std::string& event_type, const std::string& message)
    : interval_(interval), event_type_(event_type), message_(message), running_(false) {}

// Starts the timer in a separate thread.
// @param processor: Reference to the EventProcessor to send generated events.
void TimerScheduler::start(EventProcessor& processor) {
    if (running_) {
        std::cout << "[TimerScheduler] Already running with interval: " << interval_.count() << "s" << std::endl;
        return;
    }
    running_ = true; // Set the running flag to true to start the worker thread.
    std::cout << "[TimerScheduler] Starting timer with interval: " << interval_.count() << "s" << std::endl;
    // Create a new thread to run the timer logic.
    // std::ref(processor) is used to pass the EventProcessor by reference to the thread's function.
    worker_thread_ = std::thread(&TimerScheduler::runTimer, this, std::ref(processor));
}

// Stops the timer and waits for the worker thread to finish.
void TimerScheduler::stop() {
    if (!running_) {
        std::cout << "[TimerScheduler] Not running." << std::endl;
        return;
    }
    running_ = false; // Set the running flag to false to signal the worker thread to stop.
    if (worker_thread_.joinable()) {
        worker_thread_.join(); // Wait for the worker thread to complete its current cycle and exit.
    }
    std::cout << "[TimerScheduler] Stopped timer." << std::endl;
}

// The main logic for the timer, executed in a separate thread.
// It sleeps for the specified interval and then triggers an event.
// @param processor: Reference to the EventProcessor.
void TimerScheduler::runTimer(EventProcessor& processor) {
    while (running_) { // Loop continues as long as the running flag is true.
        std::this_thread::sleep_for(interval_); // Pause for the defined interval.

        if (running_) { // Check the running flag again after sleeping to ensure no event is processed if stop was called during sleep.
            std::cout << "[TimerScheduler] Timer triggered. Processing..." << std::endl;
            // Call EventProcessor to process the generated event.
            // Using processRawData for a simple string value, with "timer_message" as the key.
            processor.processRawData("timer_scheduler", event_type_, message_, "timer_message");
        }
    }
}

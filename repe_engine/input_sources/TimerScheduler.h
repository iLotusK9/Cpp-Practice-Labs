// input_sources/TimerScheduler.h
#pragma once

#include "common/IInputStrategy.h"
#include <string>
#include <thread>         // For std::thread
#include <atomic>         // For std::atomic<bool>
#include <chrono>         // For std::chrono::seconds
#include <iostream>       // For std::cout

// Forward declaration of EventProcessor
class EventProcessor;

// TimerScheduler is a concrete InputSource that triggers events periodically.
// It implements the IInputStrategy (Strategy Pattern).
class TimerScheduler : public IInputStrategy {
private:
    std::chrono::seconds interval_; // The interval between each event trigger.
    std::string event_type_;        // The type string for the generated event.
    std::string message_;           // A message string to be included in the generated event.
    std::thread worker_thread_;     // Separate thread to run the timer logic.
    std::atomic<bool> running_;     // Flag to control graceful thread shutdown.

public:
    // Constructor for TimerScheduler.
    // @param interval: The duration between each event trigger.
    // @param event_type: The type string for the generated event (e.g., "heartbeat").
    // @param message: A message string to be included in the generated event.
    TimerScheduler(std::chrono::seconds interval, const std::string& event_type, const std::string& message);

    // start() method: Starts the timer in a separate thread.
    // @param processor: Reference to the EventProcessor to send generated events.
    void start(EventProcessor& processor) override;

    // stop() method: Stops the timer and waits for the worker thread to finish.
    void stop() override;

private:
    // runTimer() function: The main logic of the timer thread.
    // It sleeps for the specified interval and then triggers an event.
    // @param processor: Reference to the EventProcessor.
    void runTimer(EventProcessor& processor);
};

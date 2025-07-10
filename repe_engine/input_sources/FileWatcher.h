// input_sources/FileWatcher.h
#pragma once

#include "common/IInputStrategy.h"
#include <string>
#include <thread>         // For std::thread
#include <atomic>         // For std::atomic<bool>
#include <fstream>        // For std::ifstream
#include <chrono>         // For std::chrono::seconds
#include <iostream>       // For std::cout, std::cerr
#include <filesystem>     // C++17 for file system operations

// Forward declaration of EventProcessor to avoid circular includes
class EventProcessor;

// FileWatcher is a concrete InputSource that monitors changes in a file.
// It implements the IInputStrategy (Strategy Pattern).
class FileWatcher : public IInputStrategy {
private:
    std::string path_;
    std::thread worker_thread_; // Separate thread to run the monitoring
    std::atomic<bool> running_; // Flag to control graceful thread shutdown
    std::string last_content_;  // Stores the last read file content to detect changes
    std::string file_content_type_; // Type of file content (e.g., "json_string", "string_value")
    std::filesystem::file_time_type last_write_time_; // Last write time of the file

public:
    // Constructor for FileWatcher
    // @param path: The path to the file to monitor.
    // @param content_type: The type of data in the file content (e.g., "json_string", "string_value").
    FileWatcher(const std::string& path, const std::string& content_type);

    // start() method: Starts monitoring the file in a separate thread.
    // @param processor: Reference to the EventProcessor to send read data.
    void start(EventProcessor& processor) override;

    // stop() method: Stops file monitoring and waits for the worker thread to finish.
    void stop() override;

private:
    // watchFile() function: The main logic of the file monitoring thread.
    // It periodically reads the file, checks for changes, and sends data to the EventProcessor.
    // @param processor: Reference to the EventProcessor.
    void watchFile(EventProcessor& processor);
};

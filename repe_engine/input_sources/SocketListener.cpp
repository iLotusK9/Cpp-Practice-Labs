// input_sources/SocketListener.cpp
#include "SocketListener.h" // Include the header for SocketListener
#include "core/EventProcessor.h" // Include EventProcessor to use it
#include <chrono> // For std::chrono::seconds, std::this_thread::sleep_for
#include <iostream> // For std::cout

// Constructor for SocketListener.
// Initializes the port and sets the running flag to false.
// @param port: The network port that the listener will bind to.
SocketListener::SocketListener(int port) : port_(port), running_(false) {}

// Starts the socket listener in a separate thread.
// @param processor: Reference to the EventProcessor to send received data.
void SocketListener::start(EventProcessor& processor) {
    if (running_) {
        std::cout << "[SocketListener] Already running on port: " << port_ << std::endl;
        return;
    }
    running_ = true; // Set the running flag to true to start the worker thread.
    std::cout << "[SocketListener] Starting to listen on port: " << port_ << std::endl;
    // Create a new thread to run the socket listening logic.
    // std::ref(processor) is used to pass the EventProcessor by reference to the thread's function.
    worker_thread_ = std::thread(&SocketListener::listenSocket, this, std::ref(processor));
}

// Stops the socket listener and waits for the worker thread to finish.
void SocketListener::stop() {
    if (!running_) {
        std::cout << "[SocketListener] Not running on port: " << port_ << std::endl;
        return;
    }
    running_ = false; // Set the running flag to false to signal the worker thread to stop.
    // In a real implementation, you would need to add logic to close the socket
    // gracefully to unblock the worker thread if it's currently blocked on I/O operations (like accept() or recv()).
    if (worker_thread_.joinable()) {
        worker_thread_.join(); // Wait for the worker thread to complete its current cycle and exit.
    }
    std::cout << "[SocketListener] Stopped listening on port: " << port_ << std::endl;
}

// The main logic for the socket listener, executed in a separate thread.
// It simulates receiving data and sending it to the EventProcessor.
// @param processor: Reference to the EventProcessor.
void SocketListener::listenSocket(EventProcessor& processor) {
    // This loop continues as long as the running flag is true.
    while (running_) {
        // IMPORTANT NOTE: This is a simulated socket listener.
        // A real-world implementation would involve actual network programming using:
        // 1. Socket creation (e.g., `socket()`)
        // 2. Socket configuration (e.g., `setsockopt()`)
        // 3. Binding the socket to an address and port (e.g., `bind()`)
        // 4. Listening for incoming connections (e.g., `listen()`)
        // 5. Accepting new connections (e.g., `accept()`) - This is a blocking call.
        // 6. Reading data from the connection (e.g., `recv()` or `read()`)
        // 7. Processing the received data.
        // 8. Closing the connection (e.g., `close()` or `shutdown()`)

        // For demonstration purposes, we simulate receiving data every 5 seconds.
        std::this_thread::sleep_for(std::chrono::seconds(5));

        if (running_) { // Check the running flag again after sleeping.
                        // This ensures no data is processed if `stop()` was called during the sleep.
            std::string dummy_data = R"({"type": "network_data", "client_ip": "127.0.0.1", "bytes_received": 1024, "protocol": "TCP"})";
            std::cout << "[SocketListener] (Simulated) Received data. Processing..." << std::endl;
            // Assume the received data is a JSON string and pass it to the EventProcessor.
            processor.processRawJsonData("socket_listener/" + std::to_string(port_), dummy_data);
        }
    }
}

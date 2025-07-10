// input_sources/RestApiEndpoint.cpp
#include "RestApiEndpoint.h" // Include the header for RestApiEndpoint
#include "core/EventProcessor.h" // Include EventProcessor to use it
#include <chrono> // For std::chrono::seconds, std::this_thread::sleep_for
#include <iostream> // For std::cout

// Constructor for RestApiEndpoint.
// Initializes the port and sets the running flag to false.
// @param port: The network port that the REST API server will listen on.
RestApiEndpoint::RestApiEndpoint(int port) : port_(port), running_(false) {}

// Starts the REST API server in a separate thread.
// @param processor: Reference to the EventProcessor to send data from received requests.
void RestApiEndpoint::start(EventProcessor& processor) {
    if (running_) {
        std::cout << "[RestApiEndpoint] Already running on port: " << port_ << std::endl;
        return;
    }
    running_ = true; // Set the running flag to true to start the server thread.
    std::cout << "[RestApiEndpoint] Starting REST API server on port: " << port_ << std::endl;
    // Create a new thread to run the HTTP server logic.
    // std::ref(processor) is used to pass the EventProcessor by reference to the thread's function.
    server_thread_ = std::thread(&RestApiEndpoint::startServer, this, std::ref(processor));
}

// Stops the REST API server and waits for the server thread to finish.
void RestApiEndpoint::stop() {
    if (!running_) {
        std::cout << "[RestApiEndpoint] Not running on port: " << port_ << std::endl;
        return;
    }
    running_ = false; // Set the running flag to false to signal the server thread to stop.
    // In a real implementation, you would need to add logic to shut down the HTTP server gracefully,
    // for example, by calling a shutdown function provided by the web server framework.
    if (server_thread_.joinable()) {
        server_thread_.join(); // Wait for the server thread to complete its current tasks and exit.
    }
    std::cout << "[RestApiEndpoint] Stopped REST API server on port: " << port_ << std::endl;
}

// The main logic for the REST API server, executed in a separate thread.
// It simulates receiving HTTP requests and sending data to the EventProcessor.
// @param processor: Reference to the EventProcessor.
void RestApiEndpoint::startServer(EventProcessor& processor) {
    // This loop continues as long as the running flag is true.
    while (running_) {
        // IMPORTANT NOTE: This is a simulated REST API server.
        // A real-world implementation would involve using a C++ web server framework (e.g., Crow, Pistache, Boost.Beast, cpprestsdk).
        // The steps would typically include:
        // 1. Initializing the server instance.
        // 2. Defining routes (e.g., POST /events, GET /status).
        // 3. When an HTTP POST request is received at a specific route (e.g., /events):
        //    a. Reading the request body (assuming it's JSON).
        //    b. Calling `processor.processRawJsonData("rest_api/port", request_body);`
        //    c. Sending an HTTP response (e.g., HTTP 200 OK).
        // 4. The server would typically block here waiting for requests or for a shutdown signal.

        // For demonstration purposes, we simulate receiving an API request every 10 seconds.
        std::this_thread::sleep_for(std::chrono::seconds(10));

        if (running_) { // Check the running flag again after sleeping.
                        // This ensures no data is processed if `stop()` was called during the sleep.
            std::string dummy_request_body = R"({"type": "api_call", "endpoint": "/data", "status": "success", "user": "api_user"})";
            std::cout << "[RestApiEndpoint] (Simulated) Received API request. Processing..." << std::endl;
            // Assume the request body is a JSON string and pass it to the EventProcessor.
            processor.processRawJsonData("rest_api/" + std::to_string(port_), dummy_request_body);
        }
    }
}

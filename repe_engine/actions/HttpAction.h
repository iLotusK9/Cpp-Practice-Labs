// actions/HttpAction.h
#pragma once

#include "actions/IActionCommand.h" // Include the base interface for actions
#include <string>                   // For std::string
#include <nlohmann/json.hpp>        // For JSON configuration and payload handling
#include <iostream>                 // For demonstration purposes (e.g., std::cout)

// HttpAction is a concrete action command that sends an HTTP request.
// It implements the IActionCommand interface.
// IMPORTANT NOTE: A real-world implementation would require an actual HTTP client library
// (e.g., libcurl, Boost.Beast, cpprestsdk). This skeleton only demonstrates the structure.
class HttpAction : public IActionCommand {
private:
    std::string url_;            // The URL to which the HTTP request will be sent.
    std::string method_;         // The HTTP method (e.g., "GET", "POST", "PUT", "DELETE").
    nlohmann::json payload_template_; // A JSON object representing the payload template.
                                      // It can contain placeholders (e.g., "{temperature}")
                                      // that will be filled with Event data.

public:
    // Constructor for HttpAction.
    // It takes a JSON configuration object to initialize the URL, method, and payload template.
    // Example configuration: {"type": "http", "url": "http://example.com/alert", "method": "POST", "payload": {"status": "{status}"}}
    // @param config: The nlohmann::json object containing the action's configuration.
    HttpAction(const nlohmann::json& config);

    // Executes the HTTP action.
    // It constructs the final payload using data from the provided Event and simulates sending an HTTP request.
    // @param event: The Event object containing data that can be used to format the payload.
    void execute(const Event& event) const override;
};

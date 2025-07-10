// actions/HttpAction.cpp
#include "HttpAction.h" // Include the header for HttpAction
#include <iostream>     // For standard output/error (e.g., std::cout, std::cerr)
#include <stdexcept>    // For standard exceptions (e.g., std::runtime_error)
#include <string>       // For std::string operations

// Constructor for HttpAction.
// Initializes the URL, HTTP method, and payload template from the provided JSON configuration.
// @param config: The nlohmann::json object containing the action's configuration.
HttpAction::HttpAction(const nlohmann::json& config) {
    // Validate that 'url' and 'method' fields exist and are strings.
    if (!config.contains("url") || !config["url"].is_string() ||
        !config.contains("method") || !config["method"].is_string()) {
        throw std::runtime_error("HttpAction config must contain 'url' and 'method' as strings.");
    }
    url_ = config["url"].get<std::string>();
    method_ = config["method"].get<std::string>();

    // If a 'payload' field exists, use it as the payload template. Otherwise, default to an empty JSON object.
    if (config.contains("payload")) {
        payload_template_ = config["payload"];
    } else {
        payload_template_ = nlohmann::json::object(); // Default to an empty JSON object
    }
}

// Executes the HTTP action.
// It constructs the final payload by filling placeholders from the Event data
// and then simulates sending an HTTP request.
// @param event: The Event object containing data that can be used to format the payload.
void HttpAction::execute(const Event& event) const {
    // IMPORTANT NOTE: This is a simulated execution.
    // A real-world implementation would use an actual HTTP client library (e.g., libcurl, Boost.Beast, cpprestsdk).
    // You would initialize the client, set the URL, method, headers, and send the request.

    std::cout << "[HTTP ACTION] Simulating " << method_ << " request to: " << url_ << std::endl;

    nlohmann::json final_payload = payload_template_; // Start with the template payload

    // If the payload template is a JSON object, iterate through its items to fill placeholders.
    if (final_payload.is_object()) {
        for (auto& [key, value] : final_payload.items()) {
            // Check if the value is a string and contains placeholders (e.g., "{key_name}").
            if (value.is_string()) {
                std::string s_val = value.get<std::string>();
                size_t start_pos = s_val.find("{");
                size_t end_pos = s_val.find("}");

                // If a placeholder is found, attempt to replace it with data from the Event.
                if (start_pos != std::string::npos && end_pos != std::string::npos && end_pos > start_pos) {
                    std::string event_key = s_val.substr(start_pos + 1, end_pos - start_pos - 1);
                    auto it = event.data.find(event_key); // Find the key in the event's data map.

                    if (it != event.data.end()) {
                        // Use std::visit to convert the EventValue (std::variant) to a nlohmann::json value.
                        // nlohmann::json can be constructed directly from basic types like int, double, bool, string.
                        final_payload[key] = std::visit([](auto&& arg) -> nlohmann::json {
                            return arg;
                        }, it->second);
                    } else {
                        // If the event key is not found, replace the placeholder with a "MISSING" tag.
                        final_payload[key] = "[MISSING_" + event_key + "]";
                    }
                }
            }
            // TODO: Extend this logic to handle placeholders within nested JSON objects or arrays
            // if payload_template_ can be more complex than a flat object.
        }
    }

    std::cout << "[HTTP ACTION] Simulated Payload: " << final_payload.dump(2) << std::endl;
    // In a real application, you would:
    // 1. Construct the full HTTP request (URL, headers, body).
    // 2. Use an HTTP client library to send the request.
    // 3. Handle the HTTP response (status code, response body, errors).
}

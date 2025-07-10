// actions/LogAction.cpp
#include "LogAction.h" // Include the header for LogAction
#include <fmt/core.h>  // Using the fmt library for powerful string formatting.
                       // You would need to add fmt to your CMakeLists.txt if you use it
                       // (e.g., find_package(fmt CONFIG REQUIRED) and target_link_libraries(your_target fmt::fmt)).
                       // If not, you can implement a simpler placeholder replacement logic manually.
#include <stdexcept>   // For std::runtime_error

// Constructor for LogAction.
// Initializes the message template from the provided JSON configuration.
// @param config: The nlohmann::json object containing the action's configuration.
LogAction::LogAction(const nlohmann::json& config) {
    // Ensure the "message" field exists and is a string in the configuration.
    if (config.contains("message") && config["message"].is_string()) {
        message_template_ = config["message"].get<std::string>();
    } else {
        throw std::runtime_error("LogAction config must contain a 'message' string.");
    }
}

// Executes the log action.
// It formats the message using data from the provided Event and prints it to standard output.
// @param event: The Event object containing data that can be used to format the message.
void LogAction::execute(const Event& event) const {
    std::string formatted_message = formatMessage(event); // Format the message
    std::cout << "[LOG ACTION] " << formatted_message << std::endl; // Print the formatted message
}

// Helper function to format the message template by replacing placeholders with Event data.
// Placeholders are expected in the format {key}.
// @param event: The Event object from which to extract data for placeholders.
// @return The formatted message string.
std::string LogAction::formatMessage(const Event& event) const {
    std::string result = message_template_;
    size_t start_pos = 0;

    // Iterate through the message_template_ to find and replace {key} placeholders.
    while ((start_pos = result.find("{", start_pos)) != std::string::npos) {
        size_t end_pos = result.find("}", start_pos);
        if (end_pos != std::string::npos && end_pos > start_pos) {
            // Extract the key name from within the curly braces.
            std::string key = result.substr(start_pos + 1, end_pos - start_pos - 1);

            // Try to find the key in the event's data.
            auto it = event.data.find(key);
            if (it != event.data.end()) {
                // Use std::visit to get the value from EventValue (std::variant) and convert it to a string.
                std::string value_str = std::visit([](auto&& arg) -> std::string {
                    // Use if constexpr (C++17) for compile-time type checking.
                    if constexpr (std::is_same_v<decltype(arg), int>) {
                        return std::to_string(arg);
                    } else if constexpr (std::is_same_v<decltype(arg), double>) {
                        return std::to_string(arg); // Convert double to string
                    } else if constexpr (std::is_same_v<decltype(arg), bool>) {
                        return arg ? "true" : "false"; // Convert bool to "true" or "false"
                    } else if constexpr (std::is_same_v<decltype(arg), std::string>) {
                        return arg; // String is already a string
                    }
                    return "[Unsupported Type]"; // Fallback for unsupported types in EventValue
                }, it->second);

                // Replace the placeholder with the actual value string.
                result.replace(start_pos, end_pos - start_pos + 1, value_str);
                start_pos += value_str.length(); // Continue searching from after the replaced string.
            } else {
                // If the placeholder key is not found in event data, replace it with a "MISSING" tag.
                result.replace(start_pos, end_pos - start_pos + 1, "[MISSING_" + key + "]");
                start_pos = end_pos + 1; // Continue searching from after the replaced tag.
            }
        } else {
            break; // No matching '}' found, stop searching for placeholders.
        }
    }
    return result;
}

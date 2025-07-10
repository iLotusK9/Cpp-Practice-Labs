// actions/ShellCommandAction.cpp
#include "ShellCommandAction.h" // Include the header for ShellCommandAction
#include <iostream>             // For standard output/error (e.g., std::cout, std::cerr)
#include <stdexcept>            // For standard exceptions (e.g., std::runtime_error)
#include <string>               // For std::string operations
#include <cstdlib>              // For std::system (to execute shell commands)

// Constructor for ShellCommandAction.
// Initializes the command template from the provided JSON configuration.
// @param config: The nlohmann::json object containing the action's configuration.
ShellCommandAction::ShellCommandAction(const nlohmann::json& config) {
    // Ensure the "command" field exists and is a string in the configuration.
    if (config.contains("command") && config["command"].is_string()) {
        command_template_ = config["command"].get<std::string>();
    } else {
        throw std::runtime_error("ShellCommandAction config must contain a 'command' string.");
    }
}

// Executes the shell command.
// It formats the command string by filling placeholders from the Event data and then executes it.
// @param event: The Event object containing data that can be used to format the command.
void ShellCommandAction::execute(const Event& event) const {
    std::string final_command = formatCommand(event); // Format the command string
    std::cout << "[SHELL ACTION] Executing command: " << final_command << std::endl;

    // WARNING: Executing arbitrary shell commands from untrusted input can be a severe security risk (Command Injection).
    // In a production environment, it is CRUCIAL to implement strict sanitization,
    // whitelisting, or use safer methods for running subprocesses (e.g., Boost.Process).
    // The std::system() function directly invokes the system's command processor.
    int result = std::system(final_command.c_str());
    if (result != 0) {
        std::cerr << "[SHELL ACTION ERROR] Command failed with exit code: " << result << std::endl;
    }
}

// Helper function to format the command template by replacing placeholders with Event data.
// Placeholders are expected in the format {key}.
// @param event: The Event object from which to extract data for placeholders.
// @return The formatted command string.
std::string ShellCommandAction::formatCommand(const Event& event) const {
    std::string result = command_template_;
    size_t start_pos = 0;

    // Iterate through the command_template_ to find and replace {key} placeholders.
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

                // WARNING: This is the critical point for security.
                // If 'value_str' comes from untrusted user input, it could contain malicious characters
                // (e.g., "; rm -rf /") that would be executed by the shell.
                // In a real application, you MUST sanitize 'value_str' here or use a safer subprocess API.
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

// actions/ShellCommandAction.h
#pragma once

#include "actions/IActionCommand.h" // Include the base interface for actions
#include <string>                   // For std::string
#include <nlohmann/json.hpp>        // For JSON configuration
#include <iostream>                 // For std::cout, std::cerr (for logging/debugging)
#include <cstdlib>                  // For std::system (to execute shell commands)

// ShellCommandAction is a concrete action command that executes a shell command.
// It implements the IActionCommand interface.
// WARNING: Executing arbitrary shell commands from untrusted input can be a security risk (command injection).
// In a production environment, strict sanitization or whitelisting of commands is crucial.
class ShellCommandAction : public IActionCommand {
private:
    std::string command_template_; // The shell command template, which can contain placeholders (e.g., "{filename}")

public:
    // Constructor for ShellCommandAction.
    // It takes a JSON configuration object to initialize the command template.
    // Example configuration: {"type": "shell", "command": "echo 'Alert: {message}' >> /var/log/alerts.log"}
    // @param config: The nlohmann::json object containing the action's configuration.
    ShellCommandAction(const nlohmann::json& config);

    // Executes the shell command.
    // It formats the command string by filling placeholders from the Event data and then executes it.
    // @param event: The Event object containing data that can be used to format the command.
    void execute(const Event& event) const override;

private:
    // Helper function to format the command template by replacing placeholders with Event data.
    // @param event: The Event object from which to extract data for placeholders.
    // @return The formatted command string.
    std::string formatCommand(const Event& event) const;
};

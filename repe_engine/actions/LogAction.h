// actions/LogAction.h
#pragma once

#include "actions/IActionCommand.h" // Include the base interface for actions
#include <string>                   // For std::string
#include <iostream>                 // For std::cout, or your logging framework

// LogAction is a concrete action command that logs a message to the console (or a log file).
// It implements the IActionCommand interface.
class LogAction : public IActionCommand {
private:
    std::string message_template_; // The message template, which can contain placeholders (e.g., "{temperature}")

public:
    // Constructor for LogAction.
    // It takes a JSON configuration object to initialize the message template.
    // Example configuration: {"type": "log", "message": "Alarm: Temperature ({temperature}C) detected!"}
    // @param config: The nlohmann::json object containing the action's configuration.
    LogAction(const nlohmann::json& config);

    // Executes the log action.
    // It formats the message using data from the provided Event and prints it.
    // @param event: The Event object containing data that can be used to format the message.
    void execute(const Event& event) const override;

private:
    // Helper function to format the message template by replacing placeholders with Event data.
    // @param event: The Event object from which to extract data for placeholders.
    // @return The formatted message string.
    std::string formatMessage(const Event& event) const;
};

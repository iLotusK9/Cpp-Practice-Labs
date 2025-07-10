// actions/ActionFactory.cpp
#include "ActionFactory.h" // Include the header for ActionFactory
#include "LogAction.h"     // Include concrete action headers
#include "HttpAction.h"
#include "ShellCommandAction.h"
#include <stdexcept>       // For std::runtime_error
#include <iostream>        // For debug output (std::cout, std::cerr)

// Initialize the static map of ActionCreators.
// This map stores the creation functions for each registered action type.
std::map<std::string, ActionFactory::ActionCreator> ActionFactory::creators_;

// Static method to create an IActionCommand object based on the provided JSON configuration.
// This is the core of the Factory Method Pattern.
// @param action_config: The JSON object containing the action's configuration, including its "type".
// @return A unique_ptr to the created IActionCommand object.
// @throws std::runtime_error if the configuration is invalid or the action type is unknown.
std::unique_ptr<IActionCommand> ActionFactory::createAction(const nlohmann::json& action_config) {
    // Validate that the "type" field exists and is a string in the configuration.
    if (!action_config.contains("type") || !action_config["type"].is_string()) {
        throw std::runtime_error("Action config must contain a 'type' string.");
    }

    std::string action_type = action_config["type"].get<std::string>(); // Get the action type string.

    // Find the corresponding creator function in the map.
    auto it = creators_.find(action_type);
    if (it != creators_.end()) {
        std::cout << "[ActionFactory] Creating action type: " << action_type << std::endl;
        return it->second(action_config); // Call the registered creator function.
    } else {
        // If the action type is not found in the map, it's an unknown type.
        throw std::runtime_error("Unknown action type: " + action_type);
    }
}

// Static method to register a new action type with the Factory.
// This allows for easy extensibility without modifying the factory's core logic.
// @param type_name: The string name for the action type (e.g., "log", "http").
// @param creator: The ActionCreator function associated with this type.
void ActionFactory::registerAction(const std::string& type_name, ActionFactory::ActionCreator creator) {
    creators_[type_name] = creator; // Store the creator function in the map.
    std::cout << "[ActionFactory] Registered action type: " << type_name << std::endl;
}

// Helper function to register all default action types provided by the REPE engine.
// This function should be called once during application startup (e.g., in main()).
void registerAllDefaultActions() {
    ActionFactory::registerAction("log", [](const nlohmann::json& config){
        return std::make_unique<LogAction>(config);
    });
    ActionFactory::registerAction("http", [](const nlohmann::json& config){
        return std::make_unique<HttpAction>(config);
    });
    ActionFactory::registerAction("shell", [](const nlohmann::json& config){
        return std::make_unique<ShellCommandAction>(config);
    });
    // Register other Action types here if they are added in the future.
}

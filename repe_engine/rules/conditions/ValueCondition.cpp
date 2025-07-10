// rules/conditions/ValueCondition.cpp
#include "ValueCondition.h" // Include the header for ValueCondition
#include <iostream>    // For standard output/error (e.g., std::cerr)
#include <stdexcept>   // For standard exceptions (e.g., std::runtime_error)
#include <string>      // For std::string operations
#include <algorithm>   // For std::transform (if case-insensitive comparison is needed)

// Constructor for ValueCondition.
// Initializes the key, operator, and value for the condition.
// @param key: The key of the data field in the Event to compare.
// @param op: The comparison operator (e.g., "==", ">", "<").
// @param value: The value to compare against.
ValueCondition::ValueCondition(std::string_view key, std::string_view op, const EventValue& value)
    : key_(key), op_(op), value_(value) {}

// Evaluates the condition against a given Event.
// It checks if the specified key exists in the event's data and then performs the comparison.
// @param event: The Event object to evaluate.
// @return true if the condition is met, false otherwise.
bool ValueCondition::evaluate(const Event& event) const {
    // Find the key in the event's data map.
    auto it = event.data.find(key_);
    if (it == event.data.end()) {
        // If the key is not found in the event, the condition cannot be met.
        // std::cout << "[ValueCondition] Key '" << key_ << "' not found in event. Returning false." << std::endl;
        return false;
    }

    // Perform the comparison using the found event value and the rule's value.
    return compare(it->second, value_);
}

// Helper function to perform the actual comparison between two EventValue objects.
// It handles different types stored in std::variant and performs the comparison based on the operator.
// @param event_val: The EventValue from the Event.
// @param rule_val: The EventValue from the Rule (the fixed value in the condition).
// @return true if the comparison is successful and evaluates to true, false otherwise.
bool ValueCondition::compare(const EventValue& event_val, const EventValue& rule_val) const {
    // Use std::visit to apply a lambda function to the values inside the variants.
    // This allows for type-safe comparison of different types.
    return std::visit([&](auto&& arg_event, auto&& arg_rule) -> bool {
        // First, try to compare values of the exact same type.
        if constexpr (std::is_same_v<decltype(arg_event), decltype(arg_rule)>) {
            if (op_ == "==") return arg_event == arg_rule;
            if (op_ == "!=") return arg_event != arg_rule;

            // For arithmetic types and strings, support relational operators.
            if constexpr (std::is_arithmetic_v<decltype(arg_event)> || std::is_same_v<decltype(arg_event), std::string>) {
                if (op_ == ">") return arg_event > arg_rule;
                if (op_ == "<") return arg_event < arg_rule;
                if (op_ == ">=") return arg_event >= arg_rule;
                if (op_ == "<=") return arg_event <= arg_rule;
            }
        }
        // If types are different but both are arithmetic (int vs. double),
        // convert them to double for comparison to avoid loss of precision or incorrect results.
        else if constexpr (std::is_arithmetic_v<decltype(arg_event)> && std::is_arithmetic_v<decltype(arg_rule)>) {
            double d_event = static_cast<double>(arg_event);
            double d_rule = static_cast<double>(arg_rule);
            if (op_ == "==") return d_event == d_rule;
            if (op_ == "!=") return d_event != d_rule;
            if (op_ == ">") return d_event > d_rule;
            if (op_ == "<") return d_event < d_rule;
            if (op_ == ">=") return d_event >= d_rule;
            if (op_ == "<=") return d_event <= d_rule;
        }
        // Handle other specific cross-type comparisons if necessary (e.g., string to number conversion)
        // For now, if types are incompatible and not both arithmetic, log an error and return false.
        std::cerr << "[ValueCondition ERROR] Unsupported comparison between types or operator '" << op_ << "' for these types."
                  << " Event type: " << event_val.index() << ", Rule type: " << rule_val.index() << std::endl;
        return false; // Default to false for unsupported or invalid comparisons
    }, event_val, rule_val);
}

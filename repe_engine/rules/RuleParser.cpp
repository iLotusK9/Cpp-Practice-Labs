// rules/RuleParser.cpp
#include "RuleParser.h"
#include "rules/conditions/ValueCondition.h"   // Bao gồm ValueCondition
#include "rules/conditions/LogicalConditions.h" // Bao gồm AndCondition, OrCondition, NotCondition
#include <iostream>    // For std::cerr
#include <stdexcept>   // For std::runtime_error

// Phương thức parse: Chuyển đổi một JSON object đại diện cho một quy tắc thành một đối tượng Rule.
std::unique_ptr<Rule> RuleParser::parse(const nlohmann::json& rule_json) {
    if (!rule_json.is_object()) {
        throw std::runtime_error("Rule JSON must be an object.");
    }

    // Lấy ID của quy tắc, nếu không có sẽ dùng "unknown_rule" và cảnh báo.
    std::string id = rule_json.value("id", "unknown_rule");
    if (id == "unknown_rule") {
        std::cerr << "[RuleParser WARNING] Rule JSON missing 'id' field. Assigning 'unknown_rule'." << std::endl;
    }

    // Phân tích cú pháp điều kiện của quy tắc
    std::unique_ptr<ICondition> condition = nullptr;
    if (rule_json.contains("condition") && rule_json["condition"].is_object()) {
        condition = parseCondition(rule_json["condition"]);
    } else {
        throw std::runtime_error("Rule '" + id + "' JSON missing 'condition' field or it's not an object.");
    }

    // Phân tích cú pháp các hành động liên quan đến quy tắc
    std::vector<nlohmann::json> actions_config;
    if (rule_json.contains("actions") && rule_json["actions"].is_array()) {
        for (const auto& action_json : rule_json["actions"]) {
            if (action_json.is_object()) {
                actions_config.push_back(action_json);
            } else {
                std::cerr << "[RuleParser WARNING] Action in rule '" << id << "' is not an object. Skipping." << std::endl;
            }
        }
    } else {
        std::cerr << "[RuleParser WARNING] Rule '" + id + "' JSON missing 'actions' field or it's not an array. No actions will be configured." << std::endl;
    }

    // Tạo và trả về đối tượng Rule mới
    return std::make_unique<Rule>(id, std::move(condition), std::move(actions_config));
}

// Phương thức parseCondition: Đệ quy phân tích cú pháp một JSON object đại diện cho một điều kiện.
std::unique_ptr<ICondition> RuleParser::parseCondition(const nlohmann::json& cond_json) {
    if (!cond_json.is_object()) {
        throw std::runtime_error("Condition JSON must be an object.");
    }

    // Kiểm tra các loại điều kiện logic (AND, OR, NOT)
    if (cond_json.contains("and")) {
        if (!cond_json["and"].is_array()) {
            throw std::runtime_error("AND condition must be an array.");
        }
        auto and_cond = std::make_unique<AndCondition>();
        for (const auto& sub_cond_json : cond_json["and"]) {
            and_cond->addCondition(parseCondition(sub_cond_json)); // Đệ quy parse điều kiện con
        }
        return and_cond;
    } else if (cond_json.contains("or")) {
        if (!cond_json["or"].is_array()) {
            throw std::runtime_error("OR condition must be an array.");
        }
        auto or_cond = std::make_unique<OrCondition>();
        for (const auto& sub_cond_json : cond_json["or"]) {
            or_cond->addCondition(parseCondition(sub_cond_json)); // Đệ quy parse điều kiện con
        }
        return or_cond;
    } else if (cond_json.contains("not")) {
        if (!cond_json["not"].is_object()) {
            throw std::runtime_error("NOT condition must be an object.");
        }
        return std::make_unique<NotCondition>(parseCondition(cond_json["not"])); // Đệ quy parse điều kiện con
    }
    // Nếu không phải điều kiện logic, kiểm tra điều kiện giá trị (ValueCondition)
    else if (cond_json.contains("key") && cond_json.contains("operator") && cond_json.contains("value")) {
        std::string key = cond_json["key"].get<std::string>();
        std::string op = cond_json["operator"].get<std::string>();

        // Chuyển đổi giá trị từ JSON sang EventValue (std::variant)
        EventValue value;
        if (cond_json["value"].is_number_integer()) {
            value = cond_json["value"].get<int>();
        } else if (cond_json["value"].is_number_float()) {
            value = cond_json["value"].get<double>();
        } else if (cond_json["value"].is_boolean()) {
            value = cond_json["value"].get<bool>();
        } else if (cond_json["value"].is_string()) {
            value = cond_json["value"].get<std::string>();
        } else {
            throw std::runtime_error("Unsupported value type in condition for key '" + key + "'.");
        }
        return std::make_unique<ValueCondition>(key, op, value);
    } else {
        // Nếu không khớp với bất kỳ loại điều kiện nào đã biết
        throw std::runtime_error("Unknown condition type or missing required fields in condition JSON.");
    }
}

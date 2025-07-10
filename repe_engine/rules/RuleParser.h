// rules/RuleParser.h
#pragma once

#include "rules/Rule.h"              // Bao gồm Rule để parse thành đối tượng Rule
#include "rules/conditions/ICondition.h" // Bao gồm ICondition để xây dựng cây điều kiện
#include <nlohmann/json.hpp>         // Để làm việc với định dạng JSON
#include <memory>                    // For std::unique_ptr
#include <string>
#include <vector>

// RuleParser chịu trách nhiệm phân tích cú pháp các định nghĩa quy tắc từ JSON/YAML
// và xây dựng các đối tượng Rule với cây điều kiện tương ứng.
// Đây là một phần của Interpreter Pattern (đóng vai trò là Builder/Parser).
class RuleParser {
public:
    // Phương thức tĩnh parse: Chuyển đổi một JSON object đại diện cho một quy tắc
    // thành một đối tượng Rule.
    // @param rule_json: Đối tượng JSON chứa định nghĩa quy tắc.
    // @return unique_ptr tới đối tượng Rule đã được parse.
    // @throws std::runtime_error nếu JSON không hợp lệ hoặc thiếu trường cần thiết.
    static std::unique_ptr<Rule> parse(const nlohmann::json& rule_json);

private:
    // Phương thức tĩnh parseCondition: Đệ quy phân tích cú pháp một JSON object
    // đại diện cho một điều kiện và xây dựng cây ICondition.
    // @param cond_json: Đối tượng JSON chứa định nghĩa điều kiện (có thể là điều kiện giá trị hoặc điều kiện logic).
    // @return unique_ptr tới đối tượng ICondition (có thể là ValueCondition, AndCondition, OrCondition, NotCondition).
    // @throws std::runtime_error nếu JSON điều kiện không hợp lệ hoặc không xác định được loại điều kiện.
    static std::unique_ptr<ICondition> parseCondition(const nlohmann::json& cond_json);
};

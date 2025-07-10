// rules/conditions/ValueCondition.h
#pragma once

#include "rules/conditions/ICondition.h" // Bao gồm interface điều kiện cơ sở
#include <string>     // For std::string
#include <string_view> // C++17 for efficient string passing
#include <variant>    // C++17 for EventValue (std::variant)

// ValueCondition là một điều kiện cơ bản, so sánh một giá trị trong Event
// với một giá trị cố định bằng một toán tử.
// Đây là một Terminal Expression trong Interpreter Pattern.
class ValueCondition : public ICondition {
private:
    std::string key_;       // Key của trường dữ liệu trong Event (e.g., "temperature")
    std::string op_;        // Toán tử so sánh (e.g., ">", "<", "==", "!=", ">=", "<=")
    EventValue value_;      // Giá trị để so sánh (được lưu trữ dưới dạng EventValue)

public:
    // Constructor của ValueCondition.
    // @param key: Tên key của trường dữ liệu trong Event.
    // @param op: Toán tử so sánh.
    // @param value: Giá trị để so sánh với dữ liệu trong Event.
    ValueCondition(std::string_view key, std::string_view op, const EventValue& value);

    // Phương thức evaluate: Thực hiện so sánh giá trị của trường dữ liệu trong Event
    // với giá trị của điều kiện.
    // @param event: Event cần được đánh giá.
    // @return true nếu điều kiện được đáp ứng, ngược lại false.
    bool evaluate(const Event& event) const override;

private:
    // Hàm trợ giúp để thực hiện so sánh giữa hai EventValue dựa trên toán tử.
    // @param event_val: Giá trị từ Event.
    // @param rule_val: Giá trị từ Rule (giá trị cố định của điều kiện).
    // @return true nếu so sánh đúng, ngược lại false.
    bool compare(const EventValue& event_val, const EventValue& rule_val) const;
};

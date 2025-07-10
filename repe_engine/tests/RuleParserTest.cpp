// tests/RuleParserTest.cpp
#include "gtest/gtest.h"
#include "rules/RuleParser.h"
#include "rules/Rule.h"
#include "rules/conditions/ValueCondition.h"
#include "rules/conditions/LogicalConditions.h"
#include "common/Event.h"
#include <nlohmann/json.hpp>

// Test fixture cho RuleParser
class RuleParserTest : public ::testing::Test {
protected:
    // Có thể khởi tạo các đối tượng chung nếu cần
};

// Test case: Parse một Rule đơn giản với ValueCondition
TEST_F(RuleParserTest, ParseSimpleValueRule) {
    nlohmann::json rule_json = R"(
    {
        "id": "temp_check",
        "condition": {
            "key": "temperature",
            "operator": ">",
            "value": 25.0
        },
        "actions": [
            {"type": "log", "message": "High temp!"}
        ]
    }
    )"_json;

    auto rule = RuleParser::parse(rule_json);
    ASSERT_NE(rule, nullptr);
    ASSERT_EQ(rule->getId(), "temp_check");
    ASSERT_FALSE(rule->getActionsConfig().empty());
    ASSERT_EQ(rule->getActionsConfig()[0]["type"], "log");

    // Kiểm tra điều kiện
    Event high_temp_event;
    high_temp_event.data["temperature"] = 30.0;
    ASSERT_TRUE(rule->check(high_temp_event));

    Event low_temp_event;
    low_temp_event.data["temperature"] = 20.0;
    ASSERT_FALSE(rule->check(low_temp_event));
}

// Test case: Parse Rule với điều kiện AND
TEST_F(RuleParserTest, ParseAndConditionRule) {
    nlohmann::json rule_json = R"(
    {
        "id": "complex_alert",
        "condition": {
            "and": [
                {"key": "temperature", "operator": ">", "value": 30},
                {"key": "humidity", "operator": "<", "value": 50}
            ]
        },
        "actions": []
    }
    )"_json;

    auto rule = RuleParser::parse(rule_json);
    ASSERT_NE(rule, nullptr);
    ASSERT_EQ(rule->getId(), "complex_alert");

    Event event1; // True AND True
    event1.data["temperature"] = 35;
    event1.data["humidity"] = 45;
    ASSERT_TRUE(rule->check(event1));

    Event event2; // True AND False
    event2.data["temperature"] = 35;
    event2.data["humidity"] = 55;
    ASSERT_FALSE(rule->check(event2));

    Event event3; // False AND True
    event3.data["temperature"] = 25;
    event3.data["humidity"] = 45;
    ASSERT_FALSE(rule->check(event3));
}

// Test case: Parse Rule với điều kiện OR
TEST_F(RuleParserTest, ParseOrConditionRule) {
    nlohmann::json rule_json = R"(
    {
        "id": "any_problem",
        "condition": {
            "or": [
                {"key": "status", "operator": "==", "value": "ERROR"},
                {"key": "fault_code", "operator": "!=", "value": 0}
            ]
        },
        "actions": []
    }
    )"_json;

    auto rule = RuleParser::parse(rule_json);
    ASSERT_NE(rule, nullptr);
    ASSERT_EQ(rule->getId(), "any_problem");

    Event event1; // True OR False
    event1.data["status"] = "ERROR";
    event1.data["fault_code"] = 0;
    ASSERT_TRUE(rule->check(event1));

    Event event2; // False OR True
    event2.data["status"] = "OK";
    event2.data["fault_code"] = 101;
    ASSERT_TRUE(rule->check(event2));

    Event event3; // False OR False
    event3.data["status"] = "OK";
    event3.data["fault_code"] = 0;
    ASSERT_FALSE(rule->check(event3));
}

// Test case: Parse Rule với điều kiện NOT
TEST_F(RuleParserTest, ParseNotConditionRule) {
    nlohmann::json rule_json = R"(
    {
        "id": "not_active",
        "condition": {
            "not": {"key": "is_active", "operator": "==", "value": true}
        },
        "actions": []
    }
    )"_json;

    auto rule = RuleParser::parse(rule_json);
    ASSERT_NE(rule, nullptr);
    ASSERT_EQ(rule->getId(), "not_active");

    Event event1; // NOT True -> False
    event1.data["is_active"] = true;
    ASSERT_FALSE(rule->check(event1));

    Event event2; // NOT False -> True
    event2.data["is_active"] = false;
    ASSERT_TRUE(rule->check(event2));

    Event event3; // NOT (key missing) -> True (as per ValueCondition default)
    ASSERT_TRUE(rule->check(event3));
}

// Test case: Parse Rule với điều kiện lồng nhau
TEST_F(RuleParserTest, ParseNestedConditionsRule) {
    nlohmann::json rule_json = R"(
    {
        "id": "nested_rule",
        "condition": {
            "and": [
                {"key": "level", "operator": ">", "value": 5},
                {
                    "or": [
                        {"key": "type", "operator": "==", "value": "critical"},
                        {"key": "severity", "operator": ">=", "value": 8}
                    ]
                }
            ]
        },
        "actions": []
    }
    )"_json;

    auto rule = RuleParser::parse(rule_json);
    ASSERT_NE(rule, nullptr);
    ASSERT_EQ(rule->getId(), "nested_rule");

    Event event1; // (True AND (True OR False)) -> True
    event1.data["level"] = 10;
    event1.data["type"] = "critical";
    event1.data["severity"] = 5;
    ASSERT_TRUE(rule->check(event1));

    Event event2; // (True AND (False OR True)) -> True
    event2.data["level"] = 10;
    event2.data["type"] = "warning";
    event2.data["severity"] = 9;
    ASSERT_TRUE(rule->check(event2));

    Event event3; // (True AND (False OR False)) -> False
    event3.data["level"] = 10;
    event3.data["type"] = "info";
    event3.data["severity"] = 5;
    ASSERT_FALSE(rule->check(event3));

    Event event4; // (False AND (...)) -> False
    event4.data["level"] = 3;
    event4.data["type"] = "critical";
    event4.data["severity"] = 9;
    ASSERT_FALSE(rule->check(event4));
}

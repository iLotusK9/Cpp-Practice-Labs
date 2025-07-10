// tests/ActionFactoryTest.cpp
#include "gtest/gtest.h"
#include "actions/ActionFactory.h"
#include "actions/LogAction.h"
#include "actions/HttpAction.h"
#include "actions/ShellCommandAction.h"
#include "common/Event.h"
#include <nlohmann/json.hpp>
#include <sstream> // For capturing cout output

// Khai báo hàm đăng ký tất cả các Action mặc định.
// Hàm này được định nghĩa trong actions/ActionFactory.cpp.
extern void registerAllDefaultActions();

// Test fixture cho ActionFactory.
// Đảm bảo rằng các hành động mặc định được đăng ký trước mỗi bài kiểm tra.
class ActionFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Đảm bảo các action được đăng ký trước mỗi test.
        registerAllDefaultActions();
    }
};

// Test case: Tạo LogAction.
TEST_F(ActionFactoryTest, CreateLogAction) {
    nlohmann::json config = {
        {"type", "log"},
        {"message", "Test log message"}
    };
    auto action = ActionFactory::createAction(config);
    ASSERT_NE(action, nullptr);
    // Kiểm tra xem đối tượng được tạo có đúng kiểu LogAction không.
    ASSERT_NE(dynamic_cast<LogAction*>(action.get()), nullptr);
}

// Test case: Tạo HttpAction.
TEST_F(ActionFactoryTest, CreateHttpAction) {
    nlohmann::json config = {
        {"type", "http"},
        {"url", "http://test.com/api"},
        {"method", "GET"}
    };
    auto action = ActionFactory::createAction(config);
    ASSERT_NE(action, nullptr);
    // Kiểm tra xem đối tượng được tạo có đúng kiểu HttpAction không.
    ASSERT_NE(dynamic_cast<HttpAction*>(action.get()), nullptr);
}

// Test case: Tạo ShellCommandAction.
TEST_F(ActionFactoryTest, CreateShellCommandAction) {
    nlohmann::json config = {
        {"type", "shell"},
        {"command", "ls -l"}
    };
    auto action = ActionFactory::createAction(config);
    ASSERT_NE(action, nullptr);
    // Kiểm tra xem đối tượng được tạo có đúng kiểu ShellCommandAction không.
    ASSERT_NE(dynamic_cast<ShellCommandAction*>(action.get()), nullptr);
}

// Test case: Tạo Action với loại không xác định.
// Mong đợi ném ra std::runtime_error.
TEST_F(ActionFactoryTest, CreateUnknownActionType) {
    nlohmann::json config = {
        {"type", "unknown_action"},
        {"message", "Some data"}
    };
    // Mong đợi ném ra runtime_error.
    ASSERT_THROW(ActionFactory::createAction(config), std::runtime_error);
}

// Test case: LogAction thực thi và định dạng thông báo.
TEST_F(ActionFactoryTest, LogActionExecution) {
    nlohmann::json config = {
        {"type", "log"},
        {"message", "Event: {event_type}, Value: {value}"}
    };
    auto action = ActionFactory::createAction(config);

    Event test_event;
    test_event.type = "sensor_reading";
    test_event.data["value"] = 123.45;

    // Chuyển hướng cout để bắt output.
    std::stringstream ss;
    std::streambuf* old_cout = std::cout.rdbuf();
    std::cout.rdbuf(ss.rdbuf());

    action->execute(test_event);

    // Khôi phục cout.
    std::cout.rdbuf(old_cout);

    // Kiểm tra xem output có chứa các chuỗi mong đợi không.
    // Lưu ý: double có thể có nhiều số thập phân, nên kiểm tra chuỗi con là phù hợp hơn.
    ASSERT_TRUE(ss.str().find("[LOG ACTION] Event: sensor_reading, Value:") != std::string::npos);
    ASSERT_TRUE(ss.str().find("123.45") != std::string::npos);
}

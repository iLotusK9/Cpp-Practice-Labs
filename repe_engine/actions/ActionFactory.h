// actions/ActionFactory.h
#pragma once

#include "actions/IActionCommand.h" // Bao gồm interface cơ sở cho các hành động
#include <memory>         // For std::unique_ptr
#include <string>         // For std::string
#include <functional>     // For std::function (để định nghĩa hàm tạo)
#include <map>            // For std::map (để lưu trữ các hàm tạo)
#include <nlohmann/json.hpp> // Để làm việc với cấu hình JSON của hành động

// ActionFactory là một Factory Pattern, chịu trách nhiệm tạo các đối tượng IActionCommand
// từ cấu hình JSON/YAML. Nó cũng hỗ trợ cơ chế đăng ký để dễ dàng mở rộng.
class ActionFactory {
public:
    // Định nghĩa kiểu hàm tạo cho các ActionCommand.
    // Một ActionCreator là một hàm nhận một nlohmann::json config và trả về một unique_ptr tới IActionCommand.
    using ActionCreator = std::function<std::unique_ptr<IActionCommand>(const nlohmann::json&)>;

    // Phương thức tĩnh createAction: Factory Method chính để tạo Action từ JSON config.
    // Nó sẽ tìm hàm tạo phù hợp dựa trên trường "type" trong action_config và gọi nó.
    // @param action_config: Đối tượng JSON chứa cấu hình của hành động cần tạo.
    // @return unique_ptr tới đối tượng IActionCommand đã được tạo.
    // @throws std::runtime_error nếu cấu hình không hợp lệ hoặc loại hành động không xác định.
    static std::unique_ptr<IActionCommand> createAction(const nlohmann::json& action_config);

    // Phương thức tĩnh registerAction: Đăng ký một loại Action mới với Factory.
    // Điều này cho phép bạn thêm các loại Action mới mà không cần sửa đổi ActionFactory.
    // @param type_name: Tên loại hành động (chuỗi) để đăng ký (ví dụ: "log", "http").
    // @param creator: Hàm tạo (ActionCreator) tương ứng với loại hành động đó.
    static void registerAction(const std::string& type_name, ActionCreator creator);

private:
    // Map tĩnh lưu trữ các hàm tạo (creator functions) cho từng loại Action.
    // Key là tên loại Action (chuỗi), Value là hàm tạo (ActionCreator).
    static std::map<std::string, ActionCreator> creators_;
};

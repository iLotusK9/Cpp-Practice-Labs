// common/Event.h
#pragma once

#include <string>
#include <map>
#include <variant>     // C++17 feature for std::variant
#include <chrono>      // For timestamp
#include <iostream>    // For debugging dump
#include <atomic>      // For generateUniqueId
#include <iomanip>     // For std::put_time

// Định nghĩa một kiểu alias cho các giá trị có thể có trong Event
// EventValue có thể chứa int, double, bool, hoặc std::string
using EventValue = std::variant<int, double, bool, std::string>;

// Định nghĩa cấu trúc Event
struct Event {
    std::string id; // ID duy nhất cho sự kiện, có thể là UUID
    std::string type; // Loại sự kiện (e.g., "sensor_reading", "user_login", "system_alert")
    std::string source; // Tên nguồn (e.g., "mqtt/temp_sensor_01", "http_api/dashboard")
    std::chrono::time_point<std::chrono::system_clock> timestamp; // Thời gian sự kiện xảy ra

    // Dữ liệu cụ thể của sự kiện dưới dạng key-value map
    std::map<std::string, EventValue> data;

    // Hàm để dễ dàng debug hoặc log Event
    std::string toString() const {
        std::string s = "Event ID: " + id + "\n";
        s += "  Type: " + type + "\n";
        s += "  Source: " + source + "\n";

        // Chuyển đổi timestamp sang chuỗi dễ đọc
        std::time_t time_t_timestamp = std::chrono::system_clock::to_time_t(timestamp);
        std::tm tm_struct = *std::localtime(&time_t_timestamp); // local time
        char time_buf[100];
        std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &tm_struct);
        s += "  Timestamp: " + std::string(time_buf) + "\n";

        s += "  Data:\n";
        for (const auto& pair : data) {
            s += "    " + pair.first + ": ";
            std::visit([&](auto&& arg) {
                // Sử dụng if constexpr (C++17) để xử lý các kiểu khác nhau trong variant
                if constexpr (std::is_same_v<decltype(arg), int>) {
                    s += std::to_string(arg);
                } else if constexpr (std::is_same_v<decltype(arg), double>) {
                    s += std::to_string(arg);
                } else if constexpr (std::is_same_v<decltype(arg), bool>) {
                    s += (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<decltype(arg), std::string>) {
                    s += "\"" + arg + "\"";
                }
            }, pair.second);
            s += "\n";
        }
        return s;
    }
};

// Hàm tiện ích để sinh ID duy nhất cho sự kiện
// Trong môi trường thực tế, nên dùng thư viện UUID (ví dụ Boost.UUID)
inline std::string generateUniqueId() {
    static std::atomic<long long> counter = 0;
    return "evt_" + std::to_string(counter++);
}

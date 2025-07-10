// core/EventProcessor.h
#pragma once

#include "common/Event.h"
#include "core/EventQueue.h"
#include <string>
#include <string_view> // C++17 feature for efficient string passing
#include <iostream>    // For logging/debug
#include <stdexcept>   // For exceptions

// Nếu bạn vẫn cần nlohmann::json để xử lý các chuỗi JSON đầu vào phức tạp
#include <nlohmann/json.hpp>

// EventProcessor chịu trách nhiệm chuẩn hóa dữ liệu thô từ các InputSource
// thành các đối tượng Event có cấu trúc và đẩy chúng vào EventQueue.
class EventProcessor {
public:
    // Constructor nhận tham chiếu đến EventQueue
    explicit EventProcessor(EventQueue& event_queue) : event_queue_(event_queue) {}

    // ---------------------------------------------------------------------
    // Quá tải (Overloads) của processRawData để xử lý các kiểu dữ liệu khác nhau
    // ---------------------------------------------------------------------

    // 1. Quá tải cho các giá trị đơn lẻ (int, double, bool, std::string)
    // Tự động chuyển đổi kiểu T sang EventValue và đặt vào map data với một key mặc định.
    template <typename T>
    void processRawData(std::string_view source_id, std::string_view event_type, const T& data_value, std::string_view data_key = "value") {
        Event event;
        event.id = generateUniqueId();
        event.source = std::string(source_id);
        event.type = std::string(event_type);
        event.timestamp = std::chrono::system_clock::now();

        try {
            // Tự động chuyển đổi T sang EventValue nhờ constructor của std::variant
            event.data[std::string(data_key)] = data_value;
            event_queue_.push(std::move(event));
            std::cout << "[EventProcessor] Event from " << source_id << " (Type: " << event_type << ", Key: " << data_key << ") processed and pushed to queue. Queue size: " << event_queue_.size() << std::endl;
        } catch (const std::bad_variant_access& e) {
            std::cerr << "[EventProcessor ERROR] Type mismatch for " << source_id << ": " << e.what() << ". Data type not supported by EventValue." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[EventProcessor ERROR] Error processing simple data from " << source_id << ": " << e.what() << std::endl;
        }
    }

    // 2. Quá tải để xử lý chuỗi JSON thô
    // Phân tích chuỗi JSON và ánh xạ các trường của nó vào Event::data.
    void processRawJsonData(std::string_view source_id, std::string_view raw_json_string) {
        Event event;
        event.id = generateUniqueId();
        event.source = std::string(source_id);
        event.timestamp = std::chrono::system_clock::now();

        try {
            nlohmann::json json_obj = nlohmann::json::parse(std::string(raw_json_string));
            event.type = json_obj.value("type", "generic_json_event"); // Lấy type từ JSON hoặc mặc định

            // Duyệt qua các cặp key-value trong JSON object và chuyển đổi sang EventValue
            for (auto it = json_obj.begin(); it != json_obj.end(); ++it) {
                // Bỏ qua các trường meta hoặc trường 'type' đã xử lý
                if (it.key() == "type" || it.key() == "_source_id" || it.key() == "_timestamp") {
                    continue;
                }

                if (it->is_number_integer()) {
                    event.data[it.key()] = it->get<int>();
                } else if (it->is_number_float()) {
                    event.data[it.key()] = it->get<double>();
                } else if (it->is_boolean()) {
                    event.data[it.key()] = it->get<bool>();
                } else if (it->is_string()) {
                    event.data[it.key()] = it->get<std::string>();
                } else {
                    // Xử lý các loại phức tạp hơn như arrays, nested objects
                    // Đối với ví dụ này, chúng ta sẽ lưu chúng dưới dạng chuỗi JSON
                    std::cerr << "[EventProcessor WARNING] Unsupported JSON type for key '" << it.key() << "'. Storing as stringified JSON." << std::endl;
                    event.data[it.key()] = it->dump();
                }
            }
            event_queue_.push(std::move(event));
            std::cout << "[EventProcessor] JSON Event from " << source_id << " processed and pushed to queue. Queue size: " << event_queue_.size() << std::endl;
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "[EventProcessor ERROR] JSON parse error for " << source_id << ": " << e.what() << ". Raw data: " << raw_json_string << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[EventProcessor ERROR] Error processing raw JSON data from " << source_id << ": " << e.what() << std::endl;
        }
    }

    // 3. Quá tải để xử lý các cấu trúc dữ liệu tùy chỉnh (struct/class)
    // Yêu cầu struct/class có hàm to_json() friend function với nlohmann::json
    template <typename T>
    void processStructuredData(std::string_view source_id, std::string_view event_type, const T& structured_data) {
        Event event;
        event.id = generateUniqueId();
        event.source = std::string(source_id);
        event.type = std::string(event_type);
        event.timestamp = std::chrono::system_clock::now();

        try {
            // Sử dụng hàm to_json của nlohmann::json để chuyển đổi T sang JSON object
            nlohmann::json j_data = structured_data;

            if (!j_data.is_object()) {
                 throw std::runtime_error("Structured data must be convertible to a JSON object.");
            }

            for (auto it = j_data.begin(); it != j_data.end(); ++it) {
                if (it->is_number_integer()) event.data[it.key()] = it->get<int>();
                else if (it->is_number_float()) event.data[it.key()] = it->get<double>();
                else if (it->is_boolean()) event.data[it.key()] = it->get<bool>();
                else if (it->is_string()) event.data[it.key()] = it->get<std::string>();
                else {
                    std::cerr << "[EventProcessor WARNING] Unsupported JSON type for key '" << it.key() << "'. Storing as stringified JSON." << std::endl;
                    event.data[it.key()] = it->dump();
                }
            }
            event_queue_.push(std::move(event));
            std::cout << "[EventProcessor] Structured Event from " << source_id << " (Type: " << event_type << ") processed and pushed to queue. Queue size: " << event_queue_.size() << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "[EventProcessor ERROR] Error processing structured data from " << source_id << ": " << e.what() << std::endl;
        }
    }

private:
    EventQueue& event_queue_; // Tham chiếu đến EventQueue
};

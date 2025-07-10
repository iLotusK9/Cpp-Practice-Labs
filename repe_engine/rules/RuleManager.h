// rules/RuleManager.h
#pragma once

#include "rules/Rule.h"
#include "rules/RuleParser.h"
#include "common/Event.h"
#include <vector>
#include <string>
#include <memory>
#include <mutex>     // For thread-safety if rules can be reloaded dynamically
#include <fstream>   // For reading rule files
#include <iostream>  // For logging

// RuleManager là một Singleton, chịu trách nhiệm quản lý và tải các quy tắc.
// Nó cung cấp một giao diện để RuleEngine đánh giá các Event dựa trên các quy tắc này.
class RuleManager {
private:
    std::vector<Rule> rules_;         // Danh sách các quy tắc đã tải
    mutable std::mutex rules_mutex_;  // Mutex để bảo vệ truy cập vào rules_ khi tải lại hoặc đánh giá

    // Private constructor để đảm bảo chỉ có một thể hiện (Singleton Pattern)
    RuleManager() = default;
    // Xóa copy constructor và assignment operator để ngăn việc sao chép thể hiện
    RuleManager(const RuleManager&) = delete;
    RuleManager& operator=(const RuleManager&) = delete;

public:
    // Phương thức tĩnh để lấy thể hiện duy nhất của RuleManager.
    // Đây là điểm truy cập toàn cục cho Singleton.
    static RuleManager& getInstance();

    // Phương thức loadRules: Tải các quy tắc từ một file cấu hình JSON.
    // Nó sẽ thay thế danh sách quy tắc hiện có bằng các quy tắc mới từ file.
    // @param config_path: Đường dẫn đến file cấu hình JSON chứa các quy tắc.
    // @throws std::runtime_error nếu không thể mở file hoặc parse JSON lỗi.
    void loadRules(const std::string& config_path);

    // Phương thức evaluate: Đánh giá một Event với tất cả các quy tắc đã tải.
    // Trả về một vector chứa cấu hình của các hành động cần thực thi
    // từ các quy tắc đã khớp.
    // @param event: Event cần được đánh giá.
    // @return Vector chứa các nlohmann::json object, mỗi object là cấu hình của một hành động.
    std::vector<nlohmann::json> evaluate(const Event& event) const;

    // Phương thức getRulesCount: Trả về số lượng quy tắc hiện có trong RuleManager.
    // @return Số lượng quy tắc.
    size_t getRulesCount() const;
};

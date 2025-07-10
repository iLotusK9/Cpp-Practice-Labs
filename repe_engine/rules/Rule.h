// rules/Rule.h
#pragma once

#include <string>
#include <memory>         // For std::unique_ptr
#include <vector>         // For std::vector
#include <nlohmann/json.hpp> // For action configuration (nlohmann::json)

#include "ICondition.h" // Bao gồm interface điều kiện

// Lớp Rule biểu diễn một quy tắc duy nhất trong hệ thống.
// Mỗi quy tắc có một ID, một điều kiện gốc (dạng cây) và một danh sách các hành động.
class Rule {
private:
    std::string id_;                        // ID duy nhất của quy tắc
    std::unique_ptr<ICondition> condition_root_; // Gốc của cây điều kiện (Interpreter Pattern)
    std::vector<nlohmann::json> actions_config_; // Cấu hình của các hành động cần thực thi khi quy tắc khớp

public:
    // Constructor của Rule.
    // @param id: ID duy nhất của quy tắc.
    // @param cond: unique_ptr tới gốc của cây điều kiện.
    // @param actions: Vector chứa cấu hình JSON của các hành động.
    Rule(const std::string& id, std::unique_ptr<ICondition> cond, std::vector<nlohmann::json> actions);

    // Phương thức check: Đánh giá xem một Event có khớp với điều kiện của quy tắc không.
    // @param event: Event cần được đánh giá.
    // @return true nếu điều kiện của quy tắc được đáp ứng, ngược lại false.
    bool check(const Event& event) const;

    // Phương thức getActionsConfig: Trả về một tham chiếu const đến vector chứa cấu hình của các hành động.
    // @return Tham chiếu const đến vector cấu hình hành động.
    const std::vector<nlohmann::json>& getActionsConfig() const;

    // Phương thức getId: Trả về ID của quy tắc.
    // @return Tham chiếu const đến chuỗi ID của quy tắc.
    const std::string& getId() const;
};

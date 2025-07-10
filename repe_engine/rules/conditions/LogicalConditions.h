// rules/conditions/LogicalConditions.h
#pragma once

#include "rules/conditions/ICondition.h" // Bao gồm interface điều kiện cơ sở
#include <vector>         // For std::vector
#include <memory>         // For std::unique_ptr

// AndCondition: Điều kiện logic AND.
// Đây là một Composite Expression trong Interpreter Pattern.
// Điều kiện này đúng nếu TẤT CẢ các điều kiện con của nó đều đúng.
class AndCondition : public ICondition {
private:
    std::vector<std::unique_ptr<ICondition>> conditions_; // Danh sách các điều kiện con

public:
    // Thêm một điều kiện con vào danh sách.
    // @param cond: unique_ptr tới điều kiện con cần thêm.
    void addCondition(std::unique_ptr<ICondition> cond);

    // Đánh giá: Tất cả các điều kiện con phải đúng.
    // @param event: Event cần được đánh giá.
    // @return true nếu tất cả điều kiện con đều đúng, ngược lại false.
    bool evaluate(const Event& event) const override;
};

// OrCondition: Điều kiện logic OR.
// Đây là một Composite Expression trong Interpreter Pattern.
// Điều kiện này đúng nếu ÍT NHẤT MỘT điều kiện con của nó đúng.
class OrCondition : public ICondition {
private:
    std::vector<std::unique_ptr<ICondition>> conditions_; // Danh sách các điều kiện con

public:
    // Thêm một điều kiện con vào danh sách.
    // @param cond: unique_ptr tới điều kiện con cần thêm.
    void addCondition(std::unique_ptr<ICondition> cond);

    // Đánh giá: Ít nhất một điều kiện con phải đúng.
    // @param event: Event cần được đánh giá.
    // @return true nếu ít nhất một điều kiện con đúng, ngược lại false.
    bool evaluate(const Event& event) const override;
};

// NotCondition: Điều kiện logic NOT.
// Đây là một Composite Expression trong Interpreter Pattern.
// Điều kiện này đúng nếu điều kiện con của nó SAI.
class NotCondition : public ICondition {
private:
    std::unique_ptr<ICondition> condition_; // Điều kiện con duy nhất

public:
    // Constructor: Nhận một điều kiện con.
    // @param cond: unique_ptr tới điều kiện con.
    NotCondition(std::unique_ptr<ICondition> cond);

    // Đánh giá: Kết quả ngược lại của điều kiện con.
    // @param event: Event cần được đánh giá.
    // @return true nếu điều kiện con sai, ngược lại false.
    bool evaluate(const Event& event) const override;
};

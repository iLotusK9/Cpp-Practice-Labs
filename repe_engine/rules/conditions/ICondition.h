// rules/conditions/ICondition.h
#pragma once

#include "common/Event.h" // Bao gồm cấu trúc Event

// ICondition là interface cho tất cả các điều kiện trong Rule Engine.
// Đây là Abstract Expression trong Interpreter Pattern.
class ICondition {
public:
    // Destructor ảo là cần thiết cho các lớp cơ sở có hàm ảo
    virtual ~ICondition() = default;

    // Phương thức evaluate: Đánh giá điều kiện dựa trên một Event.
    // Trả về true nếu điều kiện được đáp ứng, ngược lại là false.
    virtual bool evaluate(const Event& event) const = 0;
};

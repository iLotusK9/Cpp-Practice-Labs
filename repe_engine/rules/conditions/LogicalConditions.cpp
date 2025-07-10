// rules/conditions/LogicalConditions.cpp
#include "LogicalConditions.h" // Bao gồm file header của các điều kiện logic

// --- AndCondition ---
// Thêm một điều kiện con vào danh sách.
// @param cond: unique_ptr tới điều kiện con cần thêm.
void AndCondition::addCondition(std::unique_ptr<ICondition> cond) {
    conditions_.push_back(std::move(cond)); // Chuyển quyền sở hữu của unique_ptr
}

// Đánh giá: Tất cả các điều kiện con phải đúng.
// Duyệt qua tất cả các điều kiện con. Nếu bất kỳ điều kiện nào sai, trả về false ngay lập tức.
// Nếu tất cả đều đúng, trả về true.
// @param event: Event cần được đánh giá.
// @return true nếu tất cả điều kiện con đều đúng, ngược lại false.
bool AndCondition::evaluate(const Event& event) const {
    for (const auto& cond : conditions_) {
        if (!cond->evaluate(event)) {
            return false; // Nếu bất kỳ điều kiện con nào sai, trả về false
        }
    }
    return true; // Nếu tất cả điều kiện con đều đúng, trả về true
}

// --- OrCondition ---
// Thêm một điều kiện con vào danh sách.
// @param cond: unique_ptr tới điều kiện con cần thêm.
void OrCondition::addCondition(std::unique_ptr<ICondition> cond) {
    conditions_.push_back(std::move(cond)); // Chuyển quyền sở hữu của unique_ptr
}

// Đánh giá: Ít nhất một điều kiện con phải đúng.
// Duyệt qua tất cả các điều kiện con. Nếu bất kỳ điều kiện nào đúng, trả về true ngay lập tức.
// Nếu tất cả đều sai, trả về false.
// @param event: Event cần được đánh giá.
// @return true nếu ít nhất một điều kiện con đúng, ngược lại false.
bool OrCondition::evaluate(const Event& event) const {
    for (const auto& cond : conditions_) {
        if (cond->evaluate(event)) {
            return true; // Nếu bất kỳ điều kiện con nào đúng, trả về true
        }
    }
    return false; // Nếu tất cả điều kiện con đều sai, trả về false
}

// --- NotCondition ---
// Constructor: Nhận một điều kiện con.
// @param cond: unique_ptr tới điều kiện con.
NotCondition::NotCondition(std::unique_ptr<ICondition> cond)
    : condition_(std::move(cond)) {} // Chuyển quyền sở hữu của unique_ptr

// Đánh giá: Kết quả ngược lại của điều kiện con.
// Nếu điều kiện con không tồn tại, kết quả của NOT sẽ là true (giả định NOT của một điều kiện không xác định là đúng).
// @param event: Event cần được đánh giá.
// @return true nếu điều kiện con sai, ngược lại false.
bool NotCondition::evaluate(const Event& event) const {
    if (!condition_) {
        // Nếu không có điều kiện con, NOT của một điều kiện không tồn tại là không xác định.
        // Tùy thuộc vào logic nghiệp vụ, có thể ném lỗi hoặc trả về false/true mặc định.
        // Ở đây, giả định NOT của một điều kiện không tồn tại là true.
        return true;
    }
    return !condition_->evaluate(event); // Trả về kết quả ngược lại của điều kiện con
}

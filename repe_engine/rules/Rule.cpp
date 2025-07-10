// rules/Rule.cpp
#include "Rule.h" // Bao gồm file header của lớp Rule

// Constructor của Rule
// Chuyển quyền sở hữu của unique_ptr<ICondition> và vector<nlohmann::json> bằng std::move
// Điều này hiệu quả hơn so với việc sao chép (copy) khi khởi tạo các thành viên.
Rule::Rule(const std::string& id, std::unique_ptr<ICondition> cond, std::vector<nlohmann::json> actions)
    : id_(id), condition_root_(std::move(cond)), actions_config_(std::move(actions)) {}

// Phương thức check: Đánh giá điều kiện của quy tắc dựa trên một Event.
// @param event: Đối tượng Event cần được đánh giá.
// @return true nếu điều kiện của quy tắc được đáp ứng, ngược lại false.
bool Rule::check(const Event& event) const {
    // Nếu không có điều kiện gốc được định nghĩa cho quy tắc này,
    // thì quy tắc không thể được kích hoạt.
    if (!condition_root_) {
        return false;
    }
    // Gọi phương thức evaluate trên gốc của cây điều kiện (ICondition)
    // để thực hiện việc đánh giá điều kiện một cách đệ quy.
    return condition_root_->evaluate(event);
}

// Phương thức getActionsConfig: Trả về một tham chiếu const đến vector chứa cấu hình của các hành động.
// @return Tham chiếu const đến vector cấu hình hành động (nlohmann::json).
const std::vector<nlohmann::json>& Rule::getActionsConfig() const {
    return actions_config_;
}

// Phương thức getId: Trả về ID của quy tắc.
// @return Tham chiếu const đến chuỗi ID của quy tắc.
const std::string& Rule::getId() const {
    return id_;
}

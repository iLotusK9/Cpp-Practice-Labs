// rules/RuleManager.cpp
#include "RuleManager.h" // Bao gồm file header của RuleManager
#include <stdexcept>     // For std::runtime_error

// Khởi tạo thể hiện duy nhất của RuleManager.
// Static local variable đảm bảo khởi tạo một lần duy nhất (thread-safe từ C++11).
RuleManager& RuleManager::getInstance() {
    static RuleManager instance;
    return instance;
}

// Phương thức loadRules: Tải các quy tắc từ một file cấu hình JSON.
void RuleManager::loadRules(const std::string& config_path) {
    // Khóa mutex để đảm bảo an toàn luồng khi truy cập và sửa đổi danh sách quy tắc.
    std::unique_lock<std::mutex> lock(rules_mutex_);

    std::ifstream file(config_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open rules configuration file: " + config_path);
    }

    nlohmann::json rules_json;
    try {
        file >> rules_json; // Đọc toàn bộ nội dung file vào JSON object
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Failed to parse rules JSON from " + config_path + ": " + e.what());
    }

    // Đảm bảo cấu hình là một mảng JSON
    if (!rules_json.is_array()) {
        throw std::runtime_error("Rules configuration must be a JSON array of rules.");
    }

    std::vector<Rule> new_rules; // Tạo một vector tạm thời để chứa các quy tắc mới
    for (const auto& rule_json : rules_json) {
        try {
            // Sử dụng RuleParser để parse từng JSON object thành đối tượng Rule.
            // RuleParser::parse trả về unique_ptr<Rule>.
            // Dereference unique_ptr (*) tạo ra một lvalue tham chiếu đến đối tượng Rule.
            // Sử dụng std::move để chuyển lvalue này thành rvalue,
            // cho phép std::vector::push_back gọi hàm tạo di chuyển của Rule.
            new_rules.push_back(std::move(*RuleParser::parse(rule_json)));
            std::cout << "[RuleManager] Loaded rule: " << new_rules.back().getId() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[RuleManager ERROR] Failed to parse a rule: " << e.what() << ". Skipping this rule." << std::endl;
        }
    }

    // Thay thế danh sách quy tắc cũ bằng danh sách mới đã được parse thành công.
    // Sử dụng std::move để chuyển quyền sở hữu, hiệu quả hơn sao chép.
    rules_ = std::move(new_rules);
    std::cout << "[RuleManager] Successfully loaded " << rules_.size() << " rules from " << config_path << std::endl;
}

// Phương thức evaluate: Đánh giá một Event với tất cả các quy tắc đã tải.
std::vector<nlohmann::json> RuleManager::evaluate(const Event& event) const {
    std::vector<nlohmann::json> triggered_actions;
    // Khóa mutex để đảm bảo an toàn luồng khi đọc danh sách quy tắc.
    // Sử dụng unique_lock để cho phép các thao tác khác (như loadRules) có thể khóa mutex.
    std::unique_lock<std::mutex> lock(rules_mutex_);

    // Duyệt qua tất cả các quy tắc đã tải
    for (const auto& rule : rules_) {
        if (rule.check(event)) { // Kiểm tra xem quy tắc có khớp với Event không
            std::cout << "[RuleManager] Rule '" << rule.getId() << "' matched for event ID: " << event.id << std::endl;
            // Nếu quy tắc khớp, thêm tất cả các hành động của quy tắc này vào danh sách kích hoạt
            for (const auto& action_cfg : rule.getActionsConfig()) {
                triggered_actions.push_back(action_cfg);
            }
        }
    }
    return triggered_actions;
}

// Phương thức getRulesCount: Trả về số lượng quy tắc hiện có.
size_t RuleManager::getRulesCount() const {
    std::unique_lock<std::mutex> lock(rules_mutex_); // Khóa để đọc số lượng quy tắc an toàn
    return rules_.size();
}

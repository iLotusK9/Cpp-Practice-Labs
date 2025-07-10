// common/IInputStrategy.h
#pragma once

#include <string>
#include <functional> // For std::function
#include <memory>     // For std::unique_ptr

// Forward declaration của EventProcessor
// Điều này giúp tránh include vòng tròn và giảm thời gian biên dịch
class EventProcessor;

// IInputStrategy là một interface (lớp trừu tượng thuần túy)
// Nó định nghĩa giao diện chung cho tất cả các nguồn đầu vào.
// Đây là một phần của Strategy Pattern.
class IInputStrategy {
public:
    // Destructor ảo là cần thiết cho các lớp cơ sở có hàm ảo
    virtual ~IInputStrategy() = default;

    // Phương thức start() khởi động bộ lắng nghe/giám sát của nguồn đầu vào.
    // Nó nhận một tham chiếu đến EventProcessor để gửi dữ liệu đã thu thập.
    virtual void start(EventProcessor& processor) = 0;

    // Phương thức stop() dừng bộ lắng nghe/giám sát của nguồn đầu vào.
    virtual void stop() = 0;
};

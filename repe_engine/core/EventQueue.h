// core/EventQueue.h
#pragma once

#include "common/Event.h" // Bao gồm Event mới
#include <queue>              // Để sử dụng std::queue làm cấu trúc dữ liệu cơ bản
#include <mutex>              // Để đảm bảo an toàn luồng (thread-safety)
#include <condition_variable> // Để đồng bộ hóa các luồng producer/consumer
#include <optional>           // C++17 feature for std::optional

// EventQueue là một hàng đợi an toàn luồng, được sử dụng để đệm các Event.
// Đây là thành phần cốt lõi của Producer-Consumer Pattern.
class EventQueue {
public:
    // Phương thức push: Đẩy một Event vào hàng đợi.
    // Event được chuyển bằng std::move để tránh sao chép không cần thiết.
    void push(Event event) {
        std::unique_lock<std::mutex> lock(mutex_); // Khóa mutex để truy cập queue an toàn
        queue_.push(std::move(event));             // Di chuyển Event vào queue
        condition_.notify_one();                   // Thông báo cho một luồng đang chờ (nếu có) rằng có dữ liệu mới
    }

    // Phương thức pop: Lấy một Event từ hàng đợi.
    // Nếu hàng đợi trống, luồng gọi sẽ chờ cho đến khi có Event mới.
    Event pop() {
        std::unique_lock<std::mutex> lock(mutex_); // Khóa mutex
        // Chờ cho đến khi hàng đợi không trống. Hàm lambda là predicate.
        condition_.wait(lock, [this]{ return !queue_.empty(); });

        Event event = std::move(queue_.front()); // Lấy Event đầu tiên bằng cách di chuyển
        queue_.pop();                             // Xóa Event khỏi queue
        return event;
    }

    // Phương thức tryPop: Cố gắng lấy một Event từ hàng đợi mà không chờ.
    // Trả về std::optional<Event> để biểu thị có hoặc không có Event.
    std::optional<Event> tryPop() {
        std::unique_lock<std::mutex> lock(mutex_); // Khóa mutex
        if (queue_.empty()) {
            return std::nullopt; // Trả về nullopt nếu hàng đợi trống
        }
        Event event = std::move(queue_.front()); // Lấy Event đầu tiên
        queue_.pop();                             // Xóa Event khỏi queue
        return event;
    }

    // Phương thức isEmpty: Kiểm tra xem hàng đợi có trống không.
    bool isEmpty() const {
        std::unique_lock<std::mutex> lock(mutex_); // Khóa mutex
        return queue_.empty();
    }

    // Phương thức size: Lấy số lượng phần tử hiện có trong hàng đợi.
    size_t size() const {
        std::unique_lock<std::mutex> lock(mutex_); // Khóa mutex
        return queue_.size();
    }

private:
    std::queue<Event> queue_;             // Hàng đợi chứa các Event
    mutable std::mutex mutex_;            // Mutex để bảo vệ truy cập vào queue
    std::condition_variable condition_;   // Biến điều kiện để đồng bộ hóa
};

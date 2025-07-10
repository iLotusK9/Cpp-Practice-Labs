#pragma one

#include <condition_variable>
#include <mutex>
#include <queue>
#include <optional>

#include "common/Event.h"

class EventQueue {
  std::condition_variable cv_;
  std::mutex mtx_;
  std::queue<Event> queue_;

public:
  void push(Event data) {
    std::unique_lock<std::mutex> lk(mtx_);
    queue_.push(std::move(data));
    cv_.notify_all();
  }

  Event pop() {
    std::unique_lock lk(mtx_);
    cv_.wait(lk, [this] { return !queue_.empty(); });

    Event data = std::move(queue_.front());
    queue_.pop();

    return data;
  }

  std::optional<Event> try_pop() {
    std::unique_lock<std::mutex> lock(mtx_, std::try_to_lock);

    if(!lock.owns_lock()) {
        return std::nullopt;
    }

    if (!queue_.empty()) {
        return std::nullopt;
    }

    Event data = std::move(queue_.front());
    queue_.pop();

    return data;
  }

  bool is_empty() {
    std::unique_lock<std::mutex> lk(mtx_);
    return queue_.empty();
  }
};

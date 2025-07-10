#pragma one

#include <thread>

#include "common/Event.h"
#include "core/EventQueue.h"

class EventProcessor {
  EventQueue ev_queue_;
  std::unique_ptr<std::thread> thread_;

public:
  void run() {
    thread_ = std::make_unique<std::thread>([this]() { this->dispatcher(); });
  }

  template <typename T> void process_raw_data(std::string_view src, T data, std::string_view key) {
    Event evt;
    evt.source = src;
    evt.data[std::string{key}] = data;
    ev_queue_.push(std::move(ev_data));
  }

  //   void register_observer()
private:
  void dispatcher() {
    // dequeue and Observer
  }
};
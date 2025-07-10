#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <atomic>

using namespace std;

struct Task {
  int id;
  std::chrono::steady_clock::time_point last_execution_time;
  std::chrono::seconds interval;
  std::function<void(void)> func;
};

class TaskScheduler {
  std::atomic<bool> running_{true};
  std::vector<Task> task_list_;
  std::thread thread_;
  std::mutex mtx_;
  std::condition_variable cv_;

public:
  void registerTask(const Task &&task) {
    // lock task_list
    std::lock_guard<std::mutex> lock(mtx_);
    task_list_.emplace_back(task);

    // wake up run thread
    cv_.notify_one();
  }

  void start() {
    running_ = true;
    thread_ = std::thread([&]() { run(); });
  }

  void stop() {
    running_ = false;
    if (thread_.joinable())
      thread_.join();
  }

private:
  void run() {
    while (running_) {
      // lock task list
      std::lock_guard<std::mutex> lock(mtx_);

      // find next wake up tp
      auto next_wakeup_time = std::chrono::steady_clock::time_point::max();
      for (const auto &task : task_list_) {
        if (task.last_execution_time + task.interval < next_wakeup_time) {

        }
      }

      auto now = std::chrono::steady_clock::now();
      for (auto &task : task_list_) {
        if (now - task.last_execution_time >= task.interval) {
          task.func();
          task.last_execution_time = now;
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
};

int main() {
  TaskScheduler scheduler;

  scheduler.registerTask(Task{1, std::chrono::steady_clock::now(),
                              std::chrono::seconds(10),
                              []() { cout << "Task 1\n"; }});

  scheduler.start();
}
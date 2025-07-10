// action_dispatcher/ActionDispatcher.h
#pragma once

#include "actions/ActionFactory.h" // Include ActionFactory to create actions
#include "common/Event.h"         // Include Event structure
#include <vector>                 // For std::vector
#include <memory>                 // For std::unique_ptr
#include <thread>                 // For std::thread (used in ThreadPool)
#include <queue>                  // For std::queue (used in ThreadPool)
#include <mutex>                  // For std::mutex (used in ThreadPool)
#include <condition_variable>     // For std::condition_variable (used in ThreadPool)
#include <future>                 // For std::future and std::packaged_task (used in ThreadPool)
#include <functional>             // For std::function

// Simple ThreadPool class to execute tasks asynchronously.
// This helps in offloading action execution from the main RuleEngine thread.
class ThreadPool {
public:
    // Constructor: Initializes the thread pool with a specified number of worker threads.
    // @param num_threads: The number of threads in the pool.
    ThreadPool(size_t num_threads);

    // Destructor: Stops all worker threads and joins them.
    ~ThreadPool();

    // Enqueues a task (a callable object) into the thread pool's task queue.
    // The task will be executed by one of the worker threads.
    // @param f: The callable object (function, lambda, functor) to be executed.
    // @return A std::future that will hold the result of the task's execution.
    template<class F>
    auto enqueue(F&& f) -> std::future<typename std::result_of<F()>::type>;

private:
    std::vector<std::thread> workers_;           // Vector to hold the worker threads.
    std::queue<std::function<void()>> tasks_;    // Queue of tasks to be executed.
    std::mutex queue_mutex_;                     // Mutex to protect access to the task queue.
    std::condition_variable condition_;          // Condition variable to signal workers about new tasks.
    bool stop_;                                  // Flag to signal threads to stop.
};

// ActionDispatcher is a Facade that provides a simplified interface for the RuleEngine
// to request the execution of actions. It uses ActionFactory to create action commands
// and can use a ThreadPool to execute them asynchronously.
class ActionDispatcher {
private:
    ThreadPool action_thread_pool_; // Thread Pool to execute actions asynchronously.

public:
    // Constructor for ActionDispatcher.
    // @param num_threads: The number of threads for the internal ThreadPool.
    ActionDispatcher(size_t num_threads = 4); // Default to 4 threads.

    // Dispatches a list of action configurations for execution.
    // It iterates through the configurations, creates corresponding ActionCommand objects
    // using ActionFactory, and enqueues them for asynchronous execution.
    // @param actions_config: A vector of nlohmann::json objects, each representing an action's configuration.
    // @param event: The Event object that triggered these actions (passed to ActionCommand::execute).
    void dispatch(const std::vector<nlohmann::json>& actions_config, const Event& event);
};

// Template implementation for ThreadPool::enqueue.
// This needs to be in the header file because it's a template function.
template<class F>
auto ThreadPool::enqueue(F&& f) -> std::future<typename std::result_of<F()>::type> {
    // Define the return type of the packaged task.
    using return_type = typename std::result_of<F()>::type;

    // Create a packaged_task to wrap the function 'f' and get a future for its result.
    auto task = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));

    std::future<return_type> res = task->get_future(); // Get the future associated with the task.
    {
        std::unique_lock<std::mutex> lock(queue_mutex_); // Lock the queue for safe access.

        // Don't allow enqueueing tasks if the pool is stopped.
        if(stop_)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        // Add the task (wrapped in a lambda) to the queue.
        // The lambda captures the shared_ptr to the packaged_task.
        tasks_.emplace([task](){ (*task)(); });
    }
    condition_.notify_one(); // Notify one waiting worker thread that a new task is available.
    return res; // Return the future to the caller.
}

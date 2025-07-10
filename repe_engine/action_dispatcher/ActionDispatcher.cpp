// action_dispatcher/ActionDispatcher.cpp
#include "ActionDispatcher.h" // Include the header for ActionDispatcher
#include <iostream>           // For standard output/error (e.g., std::cout, std::cerr)
#include <stdexcept>          // For standard exceptions (e.g., std::runtime_error)

// --- ThreadPool Implementation ---

// Constructor for ThreadPool.
// Initializes the worker threads and sets the stop flag to false.
// @param num_threads: The number of threads to create in the pool.
ThreadPool::ThreadPool(size_t num_threads) : stop_(false) {
    // Create 'num_threads' worker threads.
    for (size_t i = 0; i < num_threads; ++i) {
        // Each worker thread runs a lambda function.
        // The lambda captures 'this' to access ThreadPool's members.
        workers_.emplace_back([this] {
            for (;;) { // Infinite loop for worker threads.
                std::function<void()> task; // Placeholder for the task to be executed.
                {
                    // Acquire a unique_lock on the queue_mutex_.
                    std::unique_lock<std::mutex> lock(this->queue_mutex_);
                    // Wait until either the stop_ flag is true or the tasks_ queue is not empty.
                    // This prevents busy-waiting and efficiently puts threads to sleep when no tasks are available.
                    this->condition_.wait(lock, [this] {
                        return this->stop_ || !this->tasks_.empty();
                    });
                    // If the stop_ flag is true and the queue is empty, the thread can exit.
                    if (this->stop_ && this->tasks_.empty())
                        return;
                    // Move the task from the front of the queue.
                    task = std::move(this->tasks_.front());
                    this->tasks_.pop(); // Remove the task from the queue.
                }
                task(); // Execute the task.
            }
        });
    }
}

// Destructor for ThreadPool.
// Signals all worker threads to stop and joins them.
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_); // Acquire lock to set stop_ flag safely.
        stop_ = true; // Set the stop flag to true.
    }
    condition_.notify_all(); // Notify all waiting worker threads to wake up and check the stop_ flag.
    // Join all worker threads to ensure they complete their current tasks and exit gracefully.
    for (std::thread &worker : workers_)
        worker.join();
}

// --- ActionDispatcher Implementation ---

// Constructor for ActionDispatcher.
// Initializes the internal ThreadPool with the specified number of threads.
// @param num_threads: The number of threads for the internal ThreadPool.
ActionDispatcher::ActionDispatcher(size_t num_threads) : action_thread_pool_(num_threads) {}

// Dispatches a list of action configurations for execution.
// It iterates through the configurations, creates corresponding ActionCommand objects
// using ActionFactory, and enqueues them for asynchronous execution in the thread pool.
// @param actions_config: A vector of nlohmann::json objects, each representing an action's configuration.
// @param event: The Event object that triggered these actions (passed to ActionCommand::execute).
void ActionDispatcher::dispatch(const std::vector<nlohmann::json>& actions_config, const Event& event) {
    for (const auto& action_cfg : actions_config) {
        try {
            // Use ActionFactory to create an ActionCommand object from the JSON configuration.
            std::unique_ptr<IActionCommand> action = ActionFactory::createAction(action_cfg);

            // Enqueue the action for asynchronous execution in the thread pool.
            // The lambda captures the unique_ptr to the action by move semantics (action = std::move(action))
            // to transfer ownership to the lambda.
            // The Event object is captured by const reference (&event) as it might be large and shared.
            action_thread_pool_.enqueue([action = std::move(action), &event]() {
                action->execute(event); // Execute the action command.
            });

        } catch (const std::exception& e) {
            // Log any errors that occur during action creation or enqueueing.
            std::cerr << "[ActionDispatcher ERROR] Error dispatching action: " << e.what() << std::endl;
            // Depending on requirements, you might want to implement more sophisticated error handling,
            // such as retries, dead-letter queues, or sending alerts.
        }
    }
}

// tests/EventQueueTest.cpp
#include "gtest/gtest.h"
#include "core/EventQueue.h"
#include "common/Event.h"
#include <thread>
#include <chrono>

// Test fixture cho EventQueue
class EventQueueTest : public ::testing::Test {
protected:
    EventQueue queue;
};

// Test case: Push và Pop một sự kiện đơn lẻ
TEST_F(EventQueueTest, PushAndPopSingleEvent) {
    Event test_event;
    test_event.id = "test_1";
    test_event.type = "simple_test";
    test_event.source = "unittest";
    test_event.data["value"] = 123;

    queue.push(test_event);
    ASSERT_FALSE(queue.isEmpty());
    ASSERT_EQ(queue.size(), 1);

    Event popped_event = queue.pop();
    ASSERT_TRUE(queue.isEmpty());
    ASSERT_EQ(queue.size(), 0);

    ASSERT_EQ(popped_event.id, "test_1");
    ASSERT_EQ(popped_event.type, "simple_test");
    ASSERT_EQ(popped_event.source, "unittest");
    ASSERT_TRUE(std::holds_alternative<int>(popped_event.data["value"]));
    ASSERT_EQ(std::get<int>(popped_event.data["value"]), 123);
}

// Test case: tryPop khi hàng đợi trống
TEST_F(EventQueueTest, TryPopEmptyQueue) {
    auto opt_event = queue.tryPop();
    ASSERT_FALSE(opt_event.has_value());
    ASSERT_TRUE(queue.isEmpty());
}

// Test case: Hoạt động của Producer-Consumer với nhiều luồng
TEST_F(EventQueueTest, MultiThreadedProducerConsumer) {
    const int num_producers = 5;
    const int num_consumers = 5;
    const int events_per_producer = 100;
    const int total_events = num_producers * events_per_producer;

    std::atomic<int> pushed_count = 0;
    std::atomic<int> popped_count = 0;

    // Luồng Producers
    std::vector<std::thread> producer_threads;
    for (int i = 0; i < num_producers; ++i) {
        producer_threads.emplace_back([&, i]() {
            for (int j = 0; j < events_per_producer; ++j) {
                Event event;
                event.id = "producer_" + std::to_string(i) + "_event_" + std::to_string(j);
                event.type = "producer_event";
                event.source = "producer_" + std::to_string(i);
                event.data["producer_id"] = i;
                event.data["event_idx"] = j;
                queue.push(event);
                pushed_count++;
                // std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Giả lập công việc
            }
        });
    }

    // Luồng Consumers
    std::vector<std::thread> consumer_threads;
    for (int i = 0; i < num_consumers; ++i) {
        consumer_threads.emplace_back([&]() {
            while (popped_count < total_events) {
                // Sử dụng tryPop để không bị block mãi mãi nếu producers đã dừng
                auto opt_event = queue.tryPop();
                if (opt_event) {
                    popped_count++;
                } else {
                    // Nếu queue trống, tạm dừng một chút để tránh vòng lặp bận
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
        });
    }

    // Chờ tất cả producers hoàn thành
    for (auto& t : producer_threads) {
        t.join();
    }

    // Đảm bảo tất cả consumers có cơ hội xử lý hết các sự kiện còn lại
    // (Có thể cần một cơ chế dừng rõ ràng hơn cho consumers trong thực tế)
    while (popped_count < total_events) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Chờ tất cả consumers hoàn thành
    for (auto& t : consumer_threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    ASSERT_EQ(pushed_count, total_events);
    ASSERT_EQ(popped_count, total_events);
    ASSERT_TRUE(queue.isEmpty());
}

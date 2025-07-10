// input_sources/RestApiEndpoint.h
#pragma once

#include "common/IInputStrategy.h"
#include <string>
#include <thread>
#include <atomic>
#include <iostream>

// Forward declaration of EventProcessor
class EventProcessor;

// RestApiEndpoint là một InputSource cụ thể, cung cấp một endpoint REST API.
// Nó triển khai IInputStrategy (Strategy Pattern).
// LƯU Ý QUAN TRỌNG: Triển khai thực tế cho một server REST API cần một framework
// web server C++ (ví dụ: Crow, Pistache, Boost.Beast, cpprestsdk).
// Đoạn mã này chỉ là một skeleton để minh họa cấu trúc và luồng.
class RestApiEndpoint : public IInputStrategy {
private:
    int port_;                  // Cổng mà server REST API sẽ lắng nghe
    std::thread server_thread_; // Luồng riêng để chạy server HTTP
    std::atomic<bool> running_; // Cờ để kiểm soát việc dừng luồng

public:
    // Constructor của RestApiEndpoint
    // @param port: Cổng mạng mà server API sẽ lắng nghe.
    RestApiEndpoint(int port);

    // Phương thức start(): Bắt đầu server REST API trong một luồng riêng.
    // @param processor: Tham chiếu đến EventProcessor để gửi dữ liệu từ các request.
    void start(EventProcessor& processor) override;

    // Phương thức stop(): Dừng server REST API và chờ luồng server kết thúc.
    void stop() override;

private:
    // Hàm startServer(): Logic chính của luồng server HTTP.
    // Nó sẽ khởi tạo server, định nghĩa các route và xử lý các request đến.
    // @param processor: Tham chiếu đến EventProcessor.
    void startServer(EventProcessor& processor);
};

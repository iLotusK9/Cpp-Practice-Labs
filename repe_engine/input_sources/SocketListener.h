// input_sources/SocketListener.h
#pragma once

#include "common/IInputStrategy.h"
#include <string>
#include <thread>
#include <atomic>
#include <iostream>

// Forward declaration của EventProcessor
class EventProcessor;

// SocketListener là một InputSource cụ thể, lắng nghe các kết nối socket.
// Nó triển khai IInputStrategy (Strategy Pattern).
// LƯU Ý QUAN TRỌNG: Triển khai thực tế cho socket cần một thư viện mạng như Boost.Asio
// hoặc các API socket của hệ điều hành (Winsock trên Windows, POSIX sockets trên Linux/macOS).
// Đoạn mã này chỉ là một skeleton để minh họa cấu trúc và luồng.
class SocketListener : public IInputStrategy {
private:
    int port_;                  // Cổng để lắng nghe
    std::thread worker_thread_; // Luồng riêng để chạy việc lắng nghe socket
    std::atomic<bool> running_; // Cờ để kiểm soát việc dừng luồng

public:
    // Constructor của SocketListener
    // @param port: Cổng mạng mà listener sẽ lắng nghe.
    SocketListener(int port);

    // Phương thức start(): Bắt đầu lắng nghe socket trong một luồng riêng.
    // @param processor: Tham chiếu đến EventProcessor để gửi dữ liệu đã nhận.
    void start(EventProcessor& processor) override;

    // Phương thức stop(): Dừng việc lắng nghe socket và chờ luồng worker kết thúc.
    void stop() override;

private:
    // Hàm listenSocket(): Logic chính của luồng lắng nghe socket.
    // Nó sẽ lắng nghe kết nối, nhận dữ liệu và gửi đến EventProcessor.
    // @param processor: Tham chiếu đến EventProcessor.
    void listenSocket(EventProcessor& processor);
};

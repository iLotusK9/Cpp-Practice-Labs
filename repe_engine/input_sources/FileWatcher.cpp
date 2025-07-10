// input_sources/FileWatcher.cpp
#include "FileWatcher.h"
#include "core/EventProcessor.h" // Cần include EventProcessor để sử dụng nó

// Constructor của FileWatcher
FileWatcher::FileWatcher(const std::string& path, const std::string& content_type)
    : path_(path), running_(false), file_content_type_(content_type) {
    // Khởi tạo last_write_time_ và last_content_ nếu file tồn tại
    // Điều này giúp tránh việc kích hoạt sự kiện ngay lập tức khi khởi động
    // nếu file đã có nội dung và không có thay đổi thực sự.
    if (std::filesystem::exists(path_)) {
        last_write_time_ = std::filesystem::last_write_time(path_);
        std::ifstream file(path_);
        if (file.is_open()) {
            last_content_ = std::string((std::istreambuf_iterator<char>(file)),
                                        std::istreambuf_iterator<char>());
            file.close();
        }
    }
}

// Phương thức start(): Khởi động luồng giám sát file.
void FileWatcher::start(EventProcessor& processor) {
    if (running_) {
        std::cout << "[FileWatcher] Already running for: " << path_ << std::endl;
        return;
    }
    running_ = true; // Đặt cờ running thành true để luồng worker bắt đầu
    std::cout << "[FileWatcher] Starting to watch: " << path_ << " (Type: " << file_content_type_ << ")" << std::endl;

    // Khởi tạo luồng worker và truyền EventProcessor bằng tham chiếu (std::ref)
    // Lambda capture 'this' để truy cập các thành viên của lớp FileWatcher.
    worker_thread_ = std::thread(&FileWatcher::watchFile, this, std::ref(processor));
}

// Phương thức stop(): Dừng luồng giám sát file.
void FileWatcher::stop() {
    if (!running_) {
        std::cout << "[FileWatcher] Not running for: " << path_ << std::endl;
        return;
    }
    running_ = false; // Đặt cờ running thành false để báo hiệu luồng worker dừng
    if (worker_thread_.joinable()) {
        worker_thread_.join(); // Chờ luồng worker kết thúc để đảm bảo tài nguyên được giải phóng
    }
    std::cout << "[FileWatcher] Stopped watching: " << path_ << std::endl;
}

// Hàm watchFile(): Logic chính chạy trong luồng riêng.
void FileWatcher::watchFile(EventProcessor& processor) {
    while (running_) { // Vòng lặp chạy cho đến khi cờ running là false
        try {
            if (std::filesystem::exists(path_)) {
                // Lấy thời gian ghi cuối cùng hiện tại của file
                std::filesystem::file_time_type current_write_time = std::filesystem::last_write_time(path_);

                // Kiểm tra nếu thời gian ghi cuối cùng thay đổi
                if (current_write_time != last_write_time_) {
                    std::ifstream file(path_);
                    if (file.is_open()) {
                        // Đọc toàn bộ nội dung file
                        std::string current_content((std::istreambuf_iterator<char>(file)),
                                                    std::istreambuf_iterator<char>());
                        file.close();

                        // Kiểm tra nếu nội dung file thực sự thay đổi (để tránh trigger khi chỉ có metadata thay đổi)
                        if (current_content != last_content_) {
                            std::cout << "[FileWatcher] File content changed. Processing..." << std::endl;
                            // Gọi EventProcessor để xử lý dữ liệu thô
                            // Sử dụng quá tải processRawJsonData hoặc processRawData tùy thuộc vào file_content_type_
                            if (file_content_type_ == "json_string") {
                                processor.processRawJsonData("file_watcher/" + path_, current_content);
                            } else if (file_content_type_ == "string_value") {
                                // Giả định event_type là "file_content_update" và key là "content"
                                processor.processRawData("file_watcher/" + path_, "file_content_update", current_content, "content");
                            } else {
                                std::cerr << "[FileWatcher ERROR] Unsupported file_content_type_: " << file_content_type_ << std::endl;
                            }

                            last_content_ = current_content; // Cập nhật nội dung cuối cùng
                            last_write_time_ = current_write_time; // Cập nhật thời gian ghi cuối cùng
                        }
                    } else {
                        std::cerr << "[FileWatcher ERROR] Could not open file: " << path_ << std::endl;
                    }
                }
            } else {
                // Có thể log thông báo nếu file không tồn tại, nhưng không phải lỗi nghiêm trọng
                // std::cerr << "[FileWatcher INFO] File does not exist: " << path_ << std::endl;
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "[FileWatcher ERROR] Filesystem error for " << path_ << ": " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[FileWatcher ERROR] General error for " << path_ << ": " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Tạm dừng 1 giây trước khi kiểm tra lại
    }
}

#pragma once

#include "common/Event.h" // Bao gồm cấu trúc Event để các hành động có thể truy cập dữ liệu sự kiện
#include <nlohmann/json.hpp> // Để truyền cấu hình hành động (nếu cần thiết cho các hành động phức tạp)

// IActionCommand là interface cho tất cả các lệnh hành động trong hệ thống REPE.
// Đây là Abstract Command trong Command Pattern.
class IActionCommand {
public:
    // Destructor ảo là cần thiết cho các lớp cơ sở có hàm ảo.
    // Điều này đảm bảo rằng các đối tượng của lớp dẫn xuất được giải phóng đúng cách
    // khi được xóa thông qua một con trỏ của lớp cơ sở.
    virtual ~IActionCommand() = default;

    // Phương thức execute: Thực hiện hành động cụ thể.
    // Nó nhận một Event để có thể sử dụng dữ liệu sự kiện trong quá trình thực hiện hành động.
    // Ví dụ: một LogAction có thể log giá trị nhiệt độ từ Event.
    // @param event: Đối tượng Event chứa dữ liệu cần thiết cho hành động.
    virtual void execute(const Event& event) const = 0;
};

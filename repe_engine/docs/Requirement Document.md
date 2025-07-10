1. Giới thiệu
Dự án Rule-based Event Processing Engine (REPE) nhằm mục đích thiết kế và triển khai một hệ thống chạy nền có khả năng nhận dữ liệu từ nhiều nguồn khác nhau, áp dụng một tập hợp các quy tắc có thể cấu hình, và thực thi các hành động tương ứng. Hệ thống này tương tự như các giải pháp tự động hóa quy tắc trong nhiều lĩnh vực như nhà thông minh, giám sát log, hoặc kích hoạt giao dịch tài chính.

2. Mục tiêu
Xây dựng một Rule Engine linh hoạt, hiệu suất cao và dễ bảo trì.

Hỗ trợ đa dạng các nguồn đầu vào và loại hành động.

Cung cấp khả năng cấu hình quy tắc động mà không cần biên dịch lại mã nguồn.

3. Tính năng chính
3.1. Input Sources (Nguồn đầu vào)
Hệ thống phải có khả năng nhận dữ liệu từ các nguồn sau:

File: Giám sát và đọc dữ liệu từ các tệp tin (ví dụ: log files, configuration files).

Socket: Lắng nghe và nhận dữ liệu qua các kết nối mạng (TCP/UDP).

REST API: Cung cấp một endpoint HTTP để các hệ thống bên ngoài có thể gửi sự kiện.

Timer (Polling): Kích hoạt sự kiện định kỳ theo lịch trình.

3.2. Event Processing (Xử lý sự kiện)
Chuẩn hóa dữ liệu: Chuyển đổi dữ liệu thô từ các nguồn khác nhau thành một định dạng sự kiện chuẩn hóa (Event object) với các trường dữ liệu được định kiểu rõ ràng (int, double, bool, string).

Hàng đợi sự kiện: Sử dụng một hàng đợi an toàn luồng để đệm và điều phối các sự kiện giữa các nguồn đầu vào và bộ xử lý quy tắc.

3.3. Rule Engine (Bộ xử lý quy tắc)
Định nghĩa quy tắc: Quy tắc phải được định nghĩa bằng một định dạng dễ đọc và cấu hình được (ưu tiên JSON/YAML).

Cấu trúc quy tắc: Mỗi quy tắc phải bao gồm:

Điều kiện (Condition): Tập hợp các tiêu chí mà một sự kiện phải đáp ứng để quy tắc được kích hoạt.

Hành động (Action): Tập hợp các hành động cần thực thi khi điều kiện được đáp ứng.

Kết hợp điều kiện: Hỗ trợ các toán tử logic để kết hợp nhiều điều kiện (AND, OR, NOT).

Tải động quy tắc: Khả năng tải lại hoặc cập nhật quy tắc trong quá trình chạy mà không cần khởi động lại hệ thống.

3.4. Action System (Hệ thống hành động)
Khi một quy tắc được kích hoạt, hệ thống phải có khả năng thực thi các loại hành động sau:

Log: Ghi lại thông tin sự kiện và hành động vào hệ thống log nội bộ hoặc bên ngoài.

Gửi HTTP: Gửi các yêu cầu HTTP (GET, POST, PUT, DELETE) đến một URL cụ thể (webhook, API).

Thực hiện Shell Command: Chạy một lệnh hệ điều hành (shell command).

Trigger Callback: Kích hoạt một hàm hoặc một sự kiện nội bộ khác trong hệ thống.

4. Yêu cầu phi chức năng
Ngôn ngữ lập trình: C++17.

Design Patterns: Áp dụng các Design Pattern phù hợp để đảm bảo tính mô-đun, khả năng mở rộng và dễ bảo trì.

Thread-Safety: Đảm bảo các thành phần hoạt động an toàn trong môi trường đa luồng.

Xử lý lỗi: Xử lý lỗi một cách mạnh mẽ ở mọi cấp độ (parsing, thực thi hành động).

Hiệu suất: Hệ thống nên có hiệu suất chấp nhận được cho việc xử lý sự kiện trong thời gian thực hoặc gần thời gian thực.
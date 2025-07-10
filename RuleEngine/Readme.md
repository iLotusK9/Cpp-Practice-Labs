🧠 C++17 và Design Pattern sử dụng:
| Kỹ thuật                               | Áp dụng cụ thể                                         |
| -------------------------------------- | ------------------------------------------------------ |
| **Template Policy**                    | Cho input (FileInput, SocketInput, ...)                |
| **std::variant / std::any**            | Cho kiểu dữ liệu trong sự kiện (`EventData`)           |
| **Factory Pattern**                    | Tạo action từ JSON hoặc YAML                           |
| **Strategy Pattern**                   | Cho các hành động: log, gửi HTTP, exec command...      |
| **Observer Pattern**                   | Dispatcher → RuleEngine                                |
| **Composite Pattern**                  | Điều kiện phức tạp: AND / OR / NOT giữa các conditions |
| **CRTP / Template**                    | Cho plugin rule evaluator                              |
| **std::function / lambda**             | Gán callback runtime                                   |
| **std::optional**                      | Trạng thái chưa chắc chắn (timeout, missing value...)  |
| **constexpr if / structured bindings** | Tối ưu hóa logic xử lý                                 |

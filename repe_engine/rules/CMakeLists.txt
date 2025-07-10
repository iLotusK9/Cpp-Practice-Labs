# rules/CMakeLists.txt
# Tạo một thư viện tĩnh từ các file nguồn của module rules
add_library(rules_lib STATIC
    Rule.cpp
    RuleParser.cpp
    RuleManager.cpp
    conditions/ValueCondition.cpp
    conditions/LogicalConditions.cpp
)

# Đảm bảo các file header được tìm thấy cho các file nguồn trong thư viện này
# PUBLIC: Các thư mục này sẽ được thêm vào include path của các target liên kết với rules_lib
target_include_directories(rules_lib PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common          # Để tìm Event.h
    ${CMAKE_SOURCE_DIR}/rules/conditions # Để tìm ICondition.h, ValueCondition.h, LogicalConditions.h
    # Nếu nlohmann/json.hpp không nằm trong common/, bạn cần thêm đường dẫn đến nó ở đây
    # Ví dụ: ${CMAKE_SOURCE_DIR}/third_party/nlohmann_json/include
)

target_link_libraries(rules_lib PRIVATE)
# Tối thiểu CMake phiên bản 3.10
cmake_minimum_required(VERSION 3.15)

# Tên dự án
project(REPE_Engine CXX)

# Bắt buộc C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Thêm các thư mục include cho toàn bộ dự án
include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/input_sources
    ${CMAKE_SOURCE_DIR}/rules
    ${CMAKE_SOURCE_DIR}/rules/conditions
    ${CMAKE_SOURCE_DIR}/actions
    ${CMAKE_SOURCE_DIR}/action_dispatcher
)

# Thêm thư viện nlohmann/json
# Bạn có thể tải nlohmann/json.hpp và đặt vào thư mục common/third_party
# Hoặc sử dụng FetchContent/CPM để quản lý dependency
# Đơn giản nhất là tải file header nlohmann/json.hpp và đặt vào một thư mục include nào đó,
# ví dụ: third_party/nlohmann_json/include/nlohmann/json.hpp
# Sau đó thêm: include_directories(${CMAKE_SOURCE_DIR}/third_party/nlohmann_json/include)
# Đối với ví dụ này, tôi sẽ giả định nlohmann/json.hpp được đặt trong common/
# (hoặc bạn có thể tải nó từ https://github.com/nlohmann/json và đặt vào một thư mục có thể include được)
# Nếu bạn đặt nlohmann/json.hpp trực tiếp vào common/, thì include_directories(common) đã đủ.
include_directories(common/third_party)

# Thêm các thư mục con
add_subdirectory(input_sources)
add_subdirectory(rules)
add_subdirectory(actions)
add_subdirectory(action_dispatcher)
# add_subdirectory(tests) # Thêm thư mục tests

# Định nghĩa các file nguồn cho ứng dụng chính
set(REPE_SOURCES
    main.cpp
)

# Tạo executable chính
add_executable(repe_engine ${REPE_SOURCES})

# Liên kết với các thư viện con đã được tạo
target_link_libraries(repe_engine
    input_sources_lib
    rules_lib
    actions_lib
    action_dispatcher_lib
)

# Để đảm bảo các thư viện được biên dịch trước executable chính
add_dependencies(repe_engine input_sources_lib rules_lib actions_lib action_dispatcher_lib)

# Cấu hình cài đặt (tùy chọn)
install(TARGETS repe_engine DESTINATION bin)

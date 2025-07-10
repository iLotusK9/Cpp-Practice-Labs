# tests/CMakeLists.txt
# Tìm kiếm Google Test
find_package(GTest CONFIG REQUIRED)

# Thêm thư mục include cho các test
include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/rules
    ${CMAKE_SOURCE_DIR}/rules/conditions
    ${CMAKE_SOURCE_DIR}/actions
    ${CMAKE_SOURCE_DIR}/action_dispatcher
)

# Định nghĩa các file nguồn cho các test
set(TEST_SOURCES
    EventQueueTest.cpp
    RuleParserTest.cpp
    ActionFactoryTest.cpp
    # Thêm các file test khác ở đây
)

# Tạo executable cho các test
add_executable(repe_tests ${TEST_SOURCES})

# Liên kết với các thư viện của dự án và Google Test
target_link_libraries(repe_tests
    input_sources_lib
    rules_lib
    actions_lib
    action_dispatcher_lib
    GTest::gtest_main # Liên kết với thư viện chính của Google Test
)

# Thêm test vào CTest (hệ thống test của CMake)
include(GoogleTest)
gtest_discover_tests(repe_tests)

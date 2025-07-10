# input_sources/CMakeLists.txt
# Tạo một thư viện tĩnh từ các file nguồn của input_sources
add_library(input_sources_lib STATIC
    FileWatcher.cpp
    SocketListener.cpp
    RestApiEndpoint.cpp
    TimerScheduler.cpp
)

# Đảm bảo các file header được tìm thấy
# PUBLIC: Các thư mục này sẽ được thêm vào include path của các target liên kết với input_sources_lib
target_include_directories(input_sources_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/core
)

cmake_minimum_required(VERSION 3.15)

project(
    repe
    VERSION 1.0
    DESCRIPTION ""
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
set(EXPORT_COMPILE_COMMANDS ON)

# 3rd libs
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)
find_package(spdlog REQUIRED)


add_subdirectory(app)

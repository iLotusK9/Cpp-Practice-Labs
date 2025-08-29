cmake_minimum_required(VERSION 3.15)

project(can_node_emu CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/core
)

include_directories(common/third_party)

set(SOURCE
    main.cpp
)

add_executable(can_node_emu ${SOURCE})

add_executable(repe main.cpp)

target_link_libraries(repe spdlog fmt nlohmann_json::nlohmann_json)
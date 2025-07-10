#include <iostream>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main ()
{
    std::cout << "Rule engine\n";
    spdlog::info("Welcome to spdlog!");
    json json;
    json["a"] = 9;

    spdlog::info("{}", json.dump());
    return 0;
}
#include <nlohmann/json.hpp>
#include <variant>

// EventData is json or std::map<string, std::variant<bool,int,string,...>>
using EventData = std::variant<int, double, bool, std::string>;
struct Event {
    std::string id; // uuid
    std::string source;
    std::map<std::string, EventData> data;
};
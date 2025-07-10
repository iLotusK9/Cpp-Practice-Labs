# actions/CMakeLists.txt
# Create a static library from the source files of the actions module.
add_library(actions_lib STATIC
    LogAction.cpp
    HttpAction.cpp
    ShellCommandAction.cpp
    ActionFactory.cpp
)

# Ensure header files are found for the source files in this library.
# PUBLIC: These directories will be added to the include path of targets linking against actions_lib.
target_include_directories(actions_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/
    ${CMAKE_SOURCE_DIR}/common # To find Event.h
    # If nlohmann/json.hpp is not in common/, you might need to add its path here.
    # Example: ${CMAKE_SOURCE_DIR}/third_party/nlohmann_json/include
)

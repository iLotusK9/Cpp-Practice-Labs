# action_dispatcher/CMakeLists.txt
# Create a static library from the source files of the action_dispatcher module.
add_library(action_dispatcher_lib STATIC
    ActionDispatcher.cpp
)

# Ensure header files are found for the source files in this library.
# PUBLIC: These directories will be added to the include path of targets linking against action_dispatcher_lib.
target_include_directories(action_dispatcher_lib PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/actions # Needed for ActionDispatcher to find ActionFactory.h and IActionCommand.h
    ${CMAKE_SOURCE_DIR}/common  # Needed for ActionDispatcher to find Event.h
)

cmake_minimum_required(VERSION 3.22)
project(GoInterpreter)

set(CMAKE_CXX_STANDARD 20)

add_executable(GoInterpreter main.cpp)

include_directories(interpreter)
add_subdirectory(interpreter)

set(CPPTERMINAL_BUILD_EXAMPLES OFF CACHE BOOL "cpp-terminal build example" FORCE)
set(CPPTERMINAL_ENABLE_TESING OFF CACHE BOOL "cpp-terminal build test" FORCE)
set(CPPTERMINAL_ENABLE_INSTALL OFF CACHE BOOL "cpp-terminal install command" FORCE)
add_subdirectory(thirdparty/cpp-terminal)

add_subdirectory(tests)

target_link_libraries(${PROJECT_NAME} interpreter cpp-terminal)

cmake_minimum_required(VERSION 3.22)
project(GoInterpreter)

set(CMAKE_CXX_STANDARD 23)

add_executable(GoInterpreter main.cpp)

include_directories(interpreter)
add_subdirectory(interpreter)

add_subdirectory(thirdparty/cpp-terminal)

add_subdirectory(tests)

target_link_libraries(${PROJECT_NAME} interpreter cpp-terminal)

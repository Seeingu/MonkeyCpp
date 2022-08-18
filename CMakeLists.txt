cmake_minimum_required(VERSION 3.22)
project(MonkeyRepl)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)

include_directories(common)
add_subdirectory(common)

include_directories(interpreter)
add_subdirectory(interpreter)

include_directories(compiler)
add_subdirectory(compiler)

set(CPPTERMINAL_BUILD_EXAMPLES OFF CACHE BOOL "cpp-terminal build example" FORCE)
set(CPPTERMINAL_ENABLE_TESING OFF CACHE BOOL "cpp-terminal build test" FORCE)
set(CPPTERMINAL_ENABLE_INSTALL OFF CACHE BOOL "cpp-terminal install command" FORCE)
add_subdirectory(thirdparty/cpp-terminal)

add_subdirectory(thirdparty/fmt)
add_subdirectory(thirdparty/magic_enum)

Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

add_subdirectory(compiler_tests)
add_subdirectory(interpreter_tests)

target_link_libraries(${PROJECT_NAME} interpreter compiler cpp-terminal)

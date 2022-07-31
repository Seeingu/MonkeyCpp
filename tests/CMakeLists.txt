Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.0.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)
#find_package(Catch2 3 REQUIRED)

# These tests can use the Catch2-provided main
add_executable(tests Lexer_test.cpp Ast_test.cpp Parser_test.cpp Evaluator_test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain interpreter)

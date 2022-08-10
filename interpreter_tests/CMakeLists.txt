project(interpreter_tests)

# These interpreter_tests can use the Catch2-provided main
add_executable(${PROJECT_NAME} Lexer_test.cpp Ast_test.cpp Parser_test.cpp Evaluator_test.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain interpreter)

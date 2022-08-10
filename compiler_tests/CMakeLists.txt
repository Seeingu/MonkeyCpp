project(compiler_tests)


add_executable(${PROJECT_NAME} Code_test.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain compiler)

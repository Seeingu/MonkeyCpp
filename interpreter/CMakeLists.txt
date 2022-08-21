project(interpreter)

set(HEADER_FILES
        Evaluator.h
        InterpreterObject.h
        Environment.h)

set(SOURCE_FILES
        Evaluator.cpp
        InterpreterObject.cpp
        Environment.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} common magic_enum fmt)

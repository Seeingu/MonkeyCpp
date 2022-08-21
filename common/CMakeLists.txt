project(common)

set(HEADER_FILES
        Lexer.h
        Token.h
        Ast.h
        Parser.h
        GIObject.h
        Builtin.h)

set(SOURCE_FILES
        Lexer.cpp
        Token.cpp
        Ast.cpp
        Parser.cpp
        GIObject.cpp
        Builtin.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} fmt magic_enum)

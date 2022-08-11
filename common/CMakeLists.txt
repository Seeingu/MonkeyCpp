project(common)

set(HEADER_FILES
        Lexer.h
        Token.h
        Ast.h
        Parser.h
        Environment.h
        GIObject.h)

set(SOURCE_FILES
        Lexer.cpp
        Token.cpp
        Ast.cpp
        Parser.cpp
        Environment.cpp
        GIObject.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})

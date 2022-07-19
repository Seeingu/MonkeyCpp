project(Interpreter)

set(HEADER_FILES
        Lexer.h
        Token.h
        Ast.h
        Parser.h
        Utils.h
        Evaluator.h
        Environment.h
        GIObject.h
        )

set(SOURCE_FILES
        Lexer.cpp
        Token.cpp
        Ast.cpp
        Utils.cpp
        Parser.cpp
        Evaluator.cpp
        Environment.cpp
        GIObject.cpp
        )

add_library(interpreter STATIC ${SOURCE_FILES} ${HEADER_FILES})

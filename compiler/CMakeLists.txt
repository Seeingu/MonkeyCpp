project(compiler)

set(HEADER_FILES
        Code.h
        Compiler.h
        VM.h
        SymbolTable.h
        CompilerObject.h
        Frame.h)

set(SOURCE_FILES
        Code.cpp
        Compiler.cpp
        VM.cpp
        SymbolTable.cpp
        CompilerObject.cpp
        Frame.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} fmt common magic_enum)

project(Compiler)

set(HEADER_FILES
        Code.h)

set(SOURCE_FILES
        Code.cpp)

add_library(compiler STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(compiler fmt::fmt)

cmake_minimum_required(VERSION 3.5.2)

add_subdirectory(Lexer)
add_subdirectory(Parser)
add_subdirectory(Interpreter)

project(SFL-lib)

set(SOURCES
    src/SFL.cpp

    include/SFL/SFL.h
)

add_library(SFL-lib ${SOURCES})

target_link_libraries(SFL-lib
    PRIVATE Interpreter
    PRIVATE Parser
    PRIVATE Lexer
)

target_include_directories(SFL-lib 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

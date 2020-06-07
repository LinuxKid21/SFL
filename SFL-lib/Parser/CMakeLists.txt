cmake_minimum_required(VERSION 3.5.2)

project(Parser)

set(SOURCES
    src/Parser.cpp

    include/Parser/Parser.h
)

add_library(Parser ${SOURCES})
target_include_directories(Parser 
    PUBLIC ./include
)
target_link_libraries(Parser
    PUBLIC Lexer
)

add_subdirectory(UnitTests)

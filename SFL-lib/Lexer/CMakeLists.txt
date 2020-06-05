cmake_minimum_required(VERSION 3.5.2)

project(Lexer)

set(SOURCES
    src/Lexer.cpp

    include/Lexer/Lexer.h
)

add_library(Lexer ${SOURCES})
target_include_directories(Lexer 
    PUBLIC ./include
)

add_subdirectory(UnitTests)

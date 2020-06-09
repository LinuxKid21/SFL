cmake_minimum_required(VERSION 3.5.2)

project(Interpreter)

set(SOURCES
    src/Interpreter.cpp
    src/InterpreterError.cpp
    src/Value.cpp

    include/Interpreter/Interpreter.h
    include/Interpreter/InterpreterError.h
    include/Interpreter/Value.h
)

add_library(Interpreter ${SOURCES})
target_link_libraries(Interpreter
    PUBLIC Parser
    PUBLIC Lexer
)
target_include_directories(Interpreter 
    PUBLIC ./include
)

add_subdirectory(UnitTests)

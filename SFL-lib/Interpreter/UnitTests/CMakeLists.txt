cmake_minimum_required(VERSION 3.5.2)

project(InterpreterUnitTests)
add_executable(InterpreterUnitTests Interpreter_test.cpp)
target_link_libraries(InterpreterUnitTests Interpreter gtest_main gmock)

add_test(NAME InterpreterUnitTests COMMAND InterpreterUnitTests)

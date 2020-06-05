cmake_minimum_required(VERSION 3.5.2)

project(LexerUnitTests)
add_executable(LexerUnitTests Lexer_test.cpp)
target_link_libraries(LexerUnitTests Lexer gtest_main gmock)

add_test(NAME LexerUnitTests COMMAND LexerUnitTests)

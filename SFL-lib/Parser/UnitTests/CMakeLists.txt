cmake_minimum_required(VERSION 3.5.2)

project(ParserUnitTests)
add_executable(ParserUnitTests Parser_test.cpp)
target_link_libraries(ParserUnitTests Parser gtest_main gmock)

add_test(NAME ParserUnitTests COMMAND ParserUnitTests)

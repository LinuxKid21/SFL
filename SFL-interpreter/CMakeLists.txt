cmake_minimum_required(VERSION 3.5.2)

project(SFL-interpreter)

set(SOURCES
    src/main.cpp
)

add_executable(SFL-interpreter ${SOURCES})
target_link_libraries(SFL-interpreter
    PUBLIC SFL-lib
)
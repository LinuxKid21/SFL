cmake_minimum_required(VERSION 3.5.2)

set(CMAKE_CXX_STANDARD 17)
project(SFL-lib)

set(SOURCES
    src/SFL.cpp

    include/SFL/SFL.h
)

add_library(SFL-lib ${SOURCES})
target_include_directories(SFL-lib 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

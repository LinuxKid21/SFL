cmake_minimum_required(VERSION 3.5.2)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

add_subdirectory(BuildGTest)
add_subdirectory(SFL-lib)
add_subdirectory(SFL-interpreter)

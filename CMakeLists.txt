cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(cpp_tools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories("include")
add_executable(test_prog app/main.cpp src/myLib.cpp)

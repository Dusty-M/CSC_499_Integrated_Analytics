cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(analytics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories("include")
add_executable(CSVParser_tester app/main.cpp src/CSVParser.cpp)
add_executable(unit_tests app/unit_tests.cpp src/CSVParser.cpp)

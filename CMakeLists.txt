cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(analytics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)

include_directories("include" ${Boost_INCLUDE_DIRS})
add_executable(CSVParser_tester app/main.cpp src/CSVParser.cpp)
add_executable(unit_tests app/unit_tests.cpp src/CSVParser.cpp)
TARGET_LINK_LIBRARIES(CSVParser_tester ${Boost_LIBRARIES})


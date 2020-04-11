cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(analytics LANGUAGES CXX)

set(PROJECT_ROOT "&{PROJECT_SOURCE_DIR}")
#include(FetchContent)
#FetchContent_Declare(
#	atlas
#	GIT_REPOSITORY https://github.com/marovira/atlas.git)
#FetchContent_Populate(atlas)
#add_subdirectory(${atlas_SOURCE_DIR} ${atlas_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
FIND_PACKAGE(Boost 1.40.0 COMPONENTS filesystem system iostreams REQUIRED)

include_directories("include")

add_executable(CSVParser_tester app/main.cpp src/CSVParser.cpp src/LeastSquaresFit.cpp)
target_link_libraries(CSVParser_tester PUBLIC Boost::program_options)

add_executable(profiler app/profiler.cpp src/CSVParser.cpp src/LeastSquaresFit.cpp)
target_link_libraries(profiler PUBLIC Boost::program_options)

add_executable(unit_tests app/unit_tests.cpp src/CSVParser.cpp src/LeastSquaresFit.cpp)

add_executable(make_plot app/make_plot.cpp src/CSVParser.cpp src/LeastSquaresFit.cpp)
#target_link_libraries(make_plot PUBLIC atlas::atlas)
target_link_libraries(make_plot PUBLIC ${Boost_LIBRARIES})


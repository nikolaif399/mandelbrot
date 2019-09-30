cmake_minimum_required (VERSION 3.1)
project(mandelbrot)

set(CMAKE_CXX_FLAGS "-std=c++17 -pthread")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/devel)

find_package( OpenCV REQUIRED)
find_package( Boost REQUIRED COMPONENTS filesystem program_options)

include_directories(
	include
	${Boost_INCLUDE_DIRS}
)

add_library(mandel_tools
	src/Mandel.cpp
)

add_executable(mandel tools/main.cpp)
target_link_libraries( mandel mandel_tools ${Boost_LIBRARIES} ${OpenCV_LIBS})

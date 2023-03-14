cmake_minimum_required (VERSION 3.1)
project(mandelbrot)

set(CMAKE_CXX_FLAGS "-std=c++17 -pthread")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/devel)

find_package( OpenCV REQUIRED)
find_package( Boost REQUIRED COMPONENTS filesystem program_options)
find_package( CUDA QUIET)

if (CUDA_FOUND)
  add_definitions(-DCUDA)
  set(CUDA_HOST_COMPILER /usr/bin/g++-6)
endif()

include_directories(
	${Boost_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

add_library(mandel_tools
	src/mandel.cpp
)

add_executable(mandel_main src/mandel_main.cpp)
target_link_libraries( mandel_main mandel_tools ${Boost_LIBRARIES} ${OpenCV_LIBS})

add_executable(mandel_benchmark src/mandel_benchmark.cpp)
target_link_libraries( mandel_benchmark mandel_tools ${Boost_LIBRARIES} ${OpenCV_LIBS})

if (CUDA_FOUND)
  message(STATUS "Cuda found! Will compile cuda kernel.")

  # Pass options to NVCC
  set(
    CUDA_NVCC_FLAGS
    ${CUDA_NVCC_FLAGS};
    -O3
  )
  cuda_add_library(
      mandel_cuda
      src/cuda_mandelbrot.cu
  )
      
    target_link_libraries( mandel_main mandel_cuda)
    target_link_libraries( mandel_benchmark mandel_cuda)

ENDIF()
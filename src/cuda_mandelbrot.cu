#include <cuComplex.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "cuda_wrapper.h"

__device__ int mandel(double x, double y, int max_count){
  double zr = 0;
  double zrtemp;
  double zi = 0;
  int count = 0;
  while(count < max_count && zr*zr + zi*zi < 4) {
    zrtemp = zr;
    zr = zr*zr - zi*zi + x;
    zi = 2*zrtemp*zi + y;
    ++count;
  }
  return count;
}

__global__ void mandel_kernel(int *iter_counts, int max_count, int width, int height, double xmin, double ymin, double xmax, double ymax)
{
  const int N = width*height;

  float dwidth = static_cast<double>(width);
  float dheight = static_cast<double>(height);
  // printf("blockIdx.x: %d    blockDim.x: %d    threadIdx.x: %d    gridDim.x: %d\n", blockIdx.x, blockDim.x, threadIdx.x, gridDim.x);

  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < N; idx += blockDim.x * gridDim.x) 
  { 
    int xind = idx % width;
    int yind = idx / width;

    double x = xmin + xind/(dwidth-1) * (xmax - xmin);
    double y = ymin + yind/(dheight-1) * (ymax - ymin); 

    iter_counts[idx] = mandel(x, y, 255);
    //printf("Index: %d, xind: %d, yind: %d, x: %.3f, y: %.3f, counts: %d\n", idx, xind, yind, x, y, iter_counts[idx]);
  }
}

MandelKernelWrapper::MandelKernelWrapper(int width, int height, int max_count) : width_(width), height_(height), max_count_(max_count) {
  const int N = width*height;
  bytes_ = N*sizeof(int);

  // Allocate memory for arrays on host.
  iter_counts_ = (int*)malloc(bytes_);

  // Initialize memory on host.
  memset(iter_counts_, 0, bytes_);

  // Allocate memory for arrays on device.
  cudaMalloc(&d_iter_counts_, bytes_);

  // Block config.
  blk_in_grid_ = ceil( float(N) / thr_per_blk_ );
}

MandelKernelWrapper::~MandelKernelWrapper() {
  // Free CPU memory.
  free(iter_counts_);

  // Free GPU memory
  cudaFree(d_iter_counts_);
}

int* MandelKernelWrapper::call_kernel(double xmin, double ymin, double xmax, double ymax) {
  // Reset iter counts.
  memset(iter_counts_, 0, bytes_);

  // Copy input data from host to device. 
  cudaMemcpy(d_iter_counts_, iter_counts_, bytes_, cudaMemcpyHostToDevice);

  // Call mandel kernel on device. 
  mandel_kernel<<< blk_in_grid_, thr_per_blk_ >>>(d_iter_counts_, max_count_, width_, height_, xmin, ymin, xmax, ymax);

  // Copy output data from device back to host.
  cudaMemcpy(iter_counts_, d_iter_counts_, bytes_, cudaMemcpyDeviceToHost);
  /*
  printf("\n---------------------------\n");
  printf("__SUCCESS__\n");
  printf("---------------------------\n");
  printf("N                 = %d\n", width_*height_);
  printf("Threads Per Block = %d\n", thr_per_blk_);
  printf("Blocks In Grid    = %d\n", blk_in_grid_);
  printf("---------------------------\n\n");
  */
  return iter_counts_;
}
#include "mandel.h"
#include <iostream>
#include <chrono>
 
int main(int argc, char** argv)
{
  constexpr int num_iters = 20;
  constexpr int width = 1000;
  constexpr int height = 1000;
  
  std::cout << "Profiling on CPU..." << std::endl;
  for (int num_threads : {1,2,4,8,16,32,64}) {
	  Mandel m(-2, -1.25, 0.5, 1.25, width, height, num_threads);

    auto start = std::chrono::steady_clock::now();
    for (int iter = 0; iter < num_iters; ++iter) {
      m.compute_grid_iter_par_cpu();
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed_time_s = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()/1e9 / num_iters;

    std::cout << "With " << num_threads << " threads: " << elapsed_time_s << " s" <<std::endl; //std::setw(4) 
  }

	#ifdef CUDA
  std::cout << "Profiling on GPU..." << std::endl;
  Mandel m(-2, -1.25, 0.5, 1.25, width, height, 1);

  auto start = std::chrono::steady_clock::now();
  for (int iter = 0; iter < num_iters; ++iter) {
    m.compute_grid_iter_par_gpu();
  }
  auto end = std::chrono::steady_clock::now();
  auto elapsed_time_s = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()/1e9 / num_iters;

  std::cout << "GPU elapsed time: " << elapsed_time_s << " s" <<std::endl; //std::setw(4) 
	#endif

	return 0;
}

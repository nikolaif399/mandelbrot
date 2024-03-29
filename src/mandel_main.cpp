#include "mandel.h"
#include <iostream>
#include <boost/program_options.hpp>
#include <chrono>

namespace po = boost::program_options;
 
int main(int argc, char** argv)
{
	po::options_description desc;
	int num_threads;
  int iters;
	desc.add_options()
		("help,h", "usage ./mandel -t num_threads")
		("num-threads,t", po::value<int>(&num_threads)->required()->default_value(1), "number of threads");
	
	po::variables_map vm;
	po::store(parse_command_line(argc,argv,desc), vm);
	po::notify(vm);

  constexpr int width = 1000; 
  constexpr int height = 1000;

	Mandel m(-2, -1.25, 0.5, 1.25, width, height, num_threads);
  #ifdef CUDA
  m.compute_grid_iter_par_gpu();
  #else
  m.compute_grid_iter_par_cpu();
  #endif	
	m.display_grid();
	return 0;
}

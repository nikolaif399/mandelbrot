#include "Mandel.h"
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
 
int main(int argc, char** argv)
{
	po::options_description desc;
	int num_threads;
	desc.add_options()
		("help", "usage ./mandel -t num_threads")
		("num-threads,t", po::value<int>(&num_threads)->required()->default_value(1), "number of threads");
	
	po::variables_map vm;
	po::store(parse_command_line(argc,argv,desc), vm);
	po::notify(vm);	

	Mandel m(-2, -1.25, 0.5, 1.25, num_threads);	
	m.compute_grid_iter_par();	
	m.display_grid();
	return 0;
}

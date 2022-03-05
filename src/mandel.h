#ifndef MANDEL_H
#define MANDEL_H

#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <algorithm>
#ifdef CUDA
#include "cuda_wrapper.h"
#endif

typedef double coord_t;

class Mandel {

public:
	Mandel(coord_t xmin, coord_t ymin, coord_t xmax, coord_t ymax, int width, int height, int num_threads = 1);
	void set_size(cv::Size grid_size);
	void compute_grid_iter_par_cpu();
  #ifdef CUDA
  void compute_grid_iter_par_gpu();
  #endif
	void display_grid();
	void save_grid(std::string filename);

private:
	static void mouse_callback(int event, int x, int y, int flags, void* mouse_callback_address);
	void compute_grid_iter_cpu(cv::Rect roi);
	std::pair<coord_t, coord_t> convert_indices_to_complex(int x_index, int y_index);
	uchar compute_cell_iter_cpu_fast(int x, int y);
  uchar compute_cell_iter_cpu_slow(int x_index, int y_index);
	
	// Parallelization
	const int num_threads_;
		
	// Mandelbrot
	uchar max_count_;
	
	// Zooming
	coord_t xmin_, ymin_, xmax_,  ymax_;
	cv::Point2i mouse_pressed_pt_;
	
  // Window size
  int width_;
  int height_;

	// OpenCV	
	std::string window_name_ = "Mandelbrot Window";	
	cv::Mat grid_, colored_grid_;

  #ifdef CUDA
  std::unique_ptr<MandelKernelWrapper> kernel_wrapper_;
  #endif
};

#endif
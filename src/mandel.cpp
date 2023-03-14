#include "mandel.h"
#include <algorithm>

#define MAINTAIN_ASPECT_RATIO

Mandel::Mandel(coord_t xmin, coord_t ymin, coord_t xmax, coord_t ymax, int width, int height, int num_threads) :
	max_count_(255),
	xmin_(xmin),
	ymin_(ymin),
	xmax_(xmax),
	ymax_(ymax),
  width_(width),
  height_(height),
	num_threads_(num_threads)
{
	grid_ = cv::Mat::zeros(width, height, CV_8UC1); // rows, cols
	cv::namedWindow(window_name_);
	cv::setMouseCallback(window_name_, mouse_callback, this);

  #ifdef CUDA
  kernel_wrapper_ = std::make_unique<MandelKernelWrapper>(width_, height_, max_count_);
  #endif
}

void Mandel::mouse_callback(int event, int x, int y, int flags, void* this_address)
{
	// Pass in address of current object, otherwise not able to access internal
	// variables in static method (required for OpenCV callback)
	Mandel* this_t = (Mandel*)this_address;
	switch(event)
	{
		case cv::EVENT_LBUTTONDOWN :
			this_t->mouse_pressed_pt_ = cv::Point2i(x,y);
			break;
		case cv::EVENT_LBUTTONUP :
			int xmin = std::min(this_t->mouse_pressed_pt_.x, x);
			int ymin = std::min(this_t->mouse_pressed_pt_.y, y);
			
			int xmax = std::max(this_t->mouse_pressed_pt_.x, x);
			int ymax = std::max(this_t->mouse_pressed_pt_.y, y);

			if (xmin == xmax && ymin == ymax)
			{	
				// Registered a User click, save high res version
				this_t->save_grid("temp1.jpg");
			}
			else
			{
				float aspect_ratio = cvRound(float(this_t->width_)/this_t->height_);
				#ifdef MAINTAIN_ASPECT_RATIO
				// Recompute xmax and ymax to ensure constant aspect ratio	
				if (ymax - ymin > xmax - xmin)
				{
					xmax = xmin + (ymax - ymin) * aspect_ratio;		
				}
				else
				{
					ymax = ymin + (xmax - xmin) / aspect_ratio;
				}
				std::cout << xmin << ", " << xmax << ", " << ymin << ", " << ymax << std::endl;
				#endif
				std::pair<coord_t, coord_t> new_min_pt = this_t->convert_indices_to_complex(xmin, ymin);
				std::pair<coord_t, coord_t> new_max_pt = this_t->convert_indices_to_complex(xmax, ymax);
				this_t->xmin_ = new_min_pt.first;
				this_t->ymin_ = new_min_pt.second;
				this_t->xmax_ = new_max_pt.first;
				this_t->ymax_ = new_max_pt.second;
				this_t->compute_grid_iter_par_gpu();
				this_t->display_grid();
			}
			break;	
	}
}

void Mandel::set_size(cv::Size grid_size)
{
	cv::Mat new_grid;
	cv::resize(grid_, new_grid, grid_size);
	grid_ = new_grid;
}

#ifdef CUDA
void Mandel::compute_grid_iter_par_gpu() {
  int *iter_counts = kernel_wrapper_->call_kernel(xmin_,ymin_, xmax_, ymax_);
  /* uchar* iter_counts_uchar = (uchar*)malloc(width_*height_*sizeof(uchar));
  for (int i = 0; i < width_*height_; ++i) {
    iter_counts_uchar[i] = iter_counts[i];
  }

  std::memcpy(grid_.data, iter_counts_uchar, width_*height_*sizeof(uchar));
  */

  for (int i = 0; i < height_; ++i)
	{
		for (int j = 0; j < width_; ++j)
		{
			grid_.at<uchar>(i,j) = iter_counts[i*width_+j];
		}
	}
  cv::applyColorMap(grid_, colored_grid_, cv::COLORMAP_JET);
}
#endif

void Mandel::compute_grid_iter_par_cpu()
{
  std::vector<std::thread> threads;
  int xStep = cvRound(width_/num_threads_);
  int yStep = height_;
  for (int i = 0; i < num_threads_ - 1; ++i)
  {
    cv::Rect r(xStep*i, 0, xStep, yStep);
    threads.push_back(std::thread(&Mandel::compute_grid_iter_cpu, this, r));
  }
  // We handle the last thread differently because it may not
  // have the same numbers of columns as the others
  int finalXStep = width_ - xStep * (num_threads_ - 1);
  cv::Rect r_final (xStep*(num_threads_-1), 0, finalXStep, yStep);
  threads.push_back(std::thread(&Mandel::compute_grid_iter_cpu, this, r_final));
  
  // Wait for threads to complete
  for (auto &t : threads)
  {
    t.join();
  }
	cv::applyColorMap(grid_, colored_grid_, cv::COLORMAP_JET);	
}

void Mandel::compute_grid_iter_cpu(cv::Rect roi)
{
	for (int x_index = roi.x; x_index < roi.x + roi.width; ++x_index)
	{
		for (int y_index = roi.y; y_index < roi.y + roi.height; ++y_index)
		{
			int cell_value = compute_cell_iter_cpu_fast(x_index, y_index);
			grid_.at<uchar>(y_index,x_index) = cell_value;
		}
	}
}

uchar Mandel::compute_cell_iter_cpu_slow(int x_index, int y_index)
{
	std::pair<coord_t, coord_t> coords = convert_indices_to_complex(x_index, y_index);
	uchar count = 0;
  
	std::complex<coord_t> z(0,0);
	std::complex<coord_t> C(coords.first, coords.second);
	while ((std::pow(z.real(),2) + std::pow(z.imag(),2)) < 4 && count < max_count_)
	{
		z = std::pow(z, 2) + C;
		++count;
	}
  return count;
}

uchar Mandel::compute_cell_iter_cpu_fast(int x_index, int y_index)
{
	std::pair<coord_t, coord_t> coords = convert_indices_to_complex(x_index, y_index);
	uchar count = 0;
  double x = coords.first;
  double y = coords.second;
  double zr = 0;
  double zrtemp;
  double zi = 0;
  while(count < max_count_ && zr*zr + zi*zi < 4) {
    zrtemp = zr;
    zr = zr*zr - zi*zi + x;
    zi = 2*zrtemp*zi + y;
    ++count;
  }
  return count;
}


std::pair<coord_t, coord_t> Mandel::convert_indices_to_complex(int x_index, int y_index)
{
	coord_t x = xmin_ + (xmax_ - xmin_) * coord_t(x_index)/width_;
	coord_t y = ymin_ + (ymax_ - ymin_) * coord_t(y_index)/height_;
	return std::make_pair(x,y);
}

void Mandel::display_grid()
{
	cv::imshow(window_name_, colored_grid_);
	cv::waitKey();
	cv::destroyAllWindows();
}

void Mandel::save_grid(std::string filename)
{
	std::cout << "Saving high resolution image to " << filename.c_str() << "... " << std::flush; 
	cv::Mat grid_temp = grid_;
	const cv::Size SAVE_SIZE(4000, 4000);
	this->set_size(SAVE_SIZE);
	this->compute_grid_iter_par_cpu();
	cv::imwrite(filename, this->colored_grid_);
	grid_ = grid_temp;
	std::cout << "Saved." << std::endl;
}

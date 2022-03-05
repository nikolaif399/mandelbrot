#include "mandel.h"
#include <algorithm>

#define MAINTAIN_ASPECT_RATIO

Mandel::Mandel(coord_t xmin, coord_t ymin, coord_t xmax, coord_t ymax, const int num_threads) :
	_max_count(255),
	_xmin(xmin),
	_ymin(ymin),
	_xmax(xmax),
	_ymax(ymax),
	_num_threads(num_threads)
{
	_grid = cv::Mat::zeros(400, 400, CV_8UC1); // rows, cols
	cv::namedWindow(_window_name);
	cv::setMouseCallback(_window_name, mouse_callback, this);
}

void Mandel::mouse_callback(int event, int x, int y, int flags, void* this_address)
{
	// Pass in address of current object, otherwise not able to access internal
	// variables in static method (required for OpenCV callback)
	Mandel* this_t = (Mandel*)this_address;
	switch(event)
	{
		case cv::EVENT_LBUTTONDOWN :
			this_t->_mouse_pressed_pt = cv::Point2i(x,y);
			break;
		case cv::EVENT_LBUTTONUP :
			int xmin = std::min(this_t->_mouse_pressed_pt.x, x);
			int ymin = std::min(this_t->_mouse_pressed_pt.y, y);
			
			int xmax = std::max(this_t->_mouse_pressed_pt.x, x);
			int ymax = std::max(this_t->_mouse_pressed_pt.y, y);

			if (xmin == xmax && ymin == ymax)
			{	
				// Registered a User click, save high res version
				this_t->save_grid("temp1.jpg");
			}
			else
			{
				float aspect_ratio = cvRound(float(this_t->_grid.cols)/this_t->_grid.rows);
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
				this_t->_xmin = new_min_pt.first;
				this_t->_ymin = new_min_pt.second;
				this_t->_xmax = new_max_pt.first;
				this_t->_ymax = new_max_pt.second;
				this_t->compute_grid_iter_par();
				this_t->display_grid();
			}
			break;	
	}
}

void Mandel::set_size(cv::Size grid_size)
{
	cv::Mat new_grid;
	cv::resize(_grid, new_grid, grid_size);
	_grid = new_grid;
}

void Mandel::compute_grid_iter_par()
{
	std::vector<std::thread> threads;
	int xStep = cvRound(_grid.cols/_num_threads);
	int yStep = _grid.rows;
	for (int i = 0; i < _num_threads - 1; ++i)
	{
		cv::Rect r(xStep*i, 0, xStep, yStep);
		threads.push_back(std::thread(&Mandel::compute_grid_iter, this, r));
	}
	// We handle the last thread differently because it may not
	// have the same numbers of columns as the others
	int finalXStep = _grid.cols - xStep * (_num_threads - 1);
	cv::Rect r_final (xStep*(_num_threads-1), 0, finalXStep, yStep);
	threads.push_back(std::thread(&Mandel::compute_grid_iter, this, r_final));
	
	// Wait for threads to complete
	for (auto &t : threads)
	{
		t.join();
	}
	cv::applyColorMap(_grid, _colored_grid, cv::COLORMAP_JET);	
}

void Mandel::compute_grid_iter(cv::Rect roi)
{
	for (int x_index = roi.x; x_index < roi.x + roi.width; ++x_index)
	{
		for (int y_index = roi.y; y_index < roi.y + roi.height; ++y_index)
		{
			int cell_value = compute_cell_iter(x_index, y_index);
			_grid.at<uchar>(y_index,x_index) = cell_value;
		}
	}
}

uchar Mandel::compute_cell_iter(int x_index, int y_index)
{
	std::pair<coord_t, coord_t> coords = convert_indices_to_complex(x_index, y_index);
	uchar count = 0;
  
  /*
	std::complex<coord_t> z(0,0);
	std::complex<coord_t> C(coords.first, coords.second);
	while ((std::pow(z.real(),2) + std::pow(z.imag(),2)) < 4 && count < _max_count)
	{
		z = std::pow(z, 2) + C;
		++count;
	}*/
	
  double x = coords.first;
  double y = coords.second;
  double zr = 0;
  double zi = 0;
  double zrtemp;
  while(count < _max_count && zr*zr + zi*zi < 4) {
    zrtemp = zr;
    zr = zr*zr - zi*zi + x;
    zi = 2*zrtemp*zi + y;
    ++count;
  }

  return count;;
}

std::pair<coord_t, coord_t> Mandel::convert_indices_to_complex(int x_index, int y_index)
{
	coord_t x = _xmin + (_xmax - _xmin) * coord_t(x_index)/_grid.cols;
	coord_t y = _ymin + (_ymax - _ymin) * coord_t(y_index)/_grid.rows;
	return std::make_pair(x,y);
}

void Mandel::display_grid()
{
	cv::imshow(_window_name, _colored_grid);
	cv::waitKey();
	cv::destroyAllWindows();
}

void Mandel::save_grid(std::string filename)
{
	std::cout << "Saving high resolution image to " << filename.c_str() << "... " << std::flush; 
	cv::Mat grid_temp = _grid;
	const cv::Size SAVE_SIZE(4000, 4000);
	this->set_size(SAVE_SIZE);
	this->compute_grid_iter_par();
	cv::imwrite(filename, this->_colored_grid);
	_grid = grid_temp;
	std::cout << "Saved." << std::endl;
}

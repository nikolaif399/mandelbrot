#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <algorithm>

typedef double coord_t;

class Mandel {

public:
	Mandel(coord_t xmin, coord_t ymin, coord_t xmax, coord_t ymax, const int num_threads = 1);
	void set_size(cv::Size grid_size);
	void compute_grid_iter_par();
	void display_grid();
	void save_grid(std::string filename);

private:
	static void mouse_callback(int event, int x, int y, int flags, void* mouse_callback_address);
	void compute_grid_iter(cv::Rect roi);
	std::pair<coord_t, coord_t> convert_indices_to_complex(int x_index, int y_index);
	uchar compute_cell_iter(int x, int y);	
	
	// Parallelization
	const int _num_threads;
		
	// Mandelbrot
	uchar _max_count;
	
	// Zooming
	coord_t _xmin, _xmax, _ymin, _ymax;
	cv::Point2i _mouse_pressed_pt;
	
	// OpenCV	
	std::string _window_name = "Mandelbrot Window";	
	cv::Mat _grid;
	cv::Mat _colored_grid;
};

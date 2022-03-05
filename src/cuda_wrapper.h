#ifndef MANDEL_CUDA_WRAPPER_H
#define MANDEL_CUDA_WRAPPER_H

class MandelKernelWrapper {
  public:
    MandelKernelWrapper(int width, int height, int max_count);

    ~MandelKernelWrapper();

    int* call_kernel(double xmin, double ymin, double xmax, double ymax);

  private:
    size_t bytes_;

    const int thr_per_blk_ = 32; // this becomes blockDim.x
    int blk_in_grid_; // this becomes gridDim.x

    int width_;
    int height_;
    int max_count_;

    int *iter_counts_;
    int *d_iter_counts_;

};

#endif
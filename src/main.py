import numpy as np
import sys
sys.path.insert(0, '../libs')
import mandelbrot
from matplotlib import pyplot as plt
from matplotlib import colors

class Application(object):
	def __init__(self):

		self.xmin = -2;
		self.xmax = 1;
		self.ymin = -1;
		self.ymax = 1;

		resolution = 200
		self.width = (self.xmax - self.xmin) * resolution
		self.height = (self.ymax - self.ymin) * resolution

		self.max_iter = 100;


	def update_img(self):
		mandel_img = mandelbrot.mandel_set(self.xmin, self.xmax, self.ymin, self.ymax, self.width, self.height, self.max_iter)
		plt.imshow(mandel_img, extent=[self.xmin, self.xmax, self.ymin, self.ymax])
		plt.show()




Application().update_img()
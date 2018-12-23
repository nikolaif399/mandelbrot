from Tkinter import *
import sys
sys.path.insert(0, '../libs')
import mandelbrot

class Application(object):
	def __init__(self):
		self.width = 500
		self.height = 500
		self.setup_root()

		self.current_window = (-1, 1, -1, 1)

	def setup_root(self):
		root = Tk()
		canvas = Canvas(root, width = self.width, height = self.height)
		canvas.pack()

		root.bind("<Key>", lambda event:
                            self.key_pressed(event))

		#hold on to references
		self.root = root
		self.canvas = canvas

	def key_pressed(self, event):
		self.mandel_update()

	def mandel_update(self):
		a = []
		row_range = self.current_window[1] - self.current_window[0]
		col_range = self.current_window[3] - self.current_window[2]
		for row in range(self.height):
			for col in range(self.width):
				i = 2 * row / (row_range) - 1 #shift to current window
				j = 2 * col / (col_range) - 1
				n = mandelbrot.iterate(i, j)
				a.append(n)

		print (max(a))
		print (min(a))


	def run(self):
		self.root.mainloop()


Application().run()
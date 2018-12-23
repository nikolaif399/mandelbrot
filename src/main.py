from Tkinter import *
import sys
sys.path.insert(0, '../libs')
import mandelbrot

class Application(object):
	def __init__(self):
		self.setup_root()

	def setup_root(self):
		root = Tk()
		w = Label(root, text="Hello, world!")
		w.pack()

		self.root = root #hold on to root reference

	def run(self):
		self.root.mainloop()


Application().run()
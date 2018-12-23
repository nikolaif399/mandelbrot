
"""
returns number of iterations if can escape (>2)
in less than 100 iterations, else returns NaN
"""
def mandelbrot(row, col):
	c = complex(row, col) #complex(real, complex)
	z = 0
	for n in range(1, 100):
		z = z**2 + c
		if abs(z) > 2:
			return n
	return NaN


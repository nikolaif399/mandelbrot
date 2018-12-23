
"""
returns number of iterations if can escape (>2)
in less than 100 iterations, else returns None
"""
def iterate(y, x):
	c = complex(y, x) #complex(real, complex)
	z = 0.1
	for n in range(1, 100):
		z = z**2 + c
		if abs(z) > 2:
			return n
	return None


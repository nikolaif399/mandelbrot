import numpy as np
#from numba import jit

"""
returns number of iterations if can escape (>2)
in less than 100 iterations, else returns None
"""

def mandel(z,maxiter):
    c = z
    for n in range(maxiter):
        if abs(z) > 2:
            return n
        z = z*z + c
    return maxiter

def mandel_set(xmin,xmax,ymin,ymax,width,height,maxiter):
    cols = np.linspace(xmin, xmax, width)
    rows = np.linspace(ymin, ymax, height)

    img = np.zeros((height, width))
    for r in range(height):
    	for c in range(width):
    		img[r][c] = mandel(complex(cols[c], rows[r]),maxiter)

    return img
import ctypes
import os

lib = ctypes.CDLL('./libpython_linker.so') #shqred library

# declqre the multiply function: int multiply(int value)
lib.multiply.argtypes = [ctypes.c_int]
lib.multiply.restype = ctypes.c_int

# declare the loopback function: int loopback()
lib.loopback.argtypes = []
lib.loopback.restype = ctypes.c_int

# declare the offset function: int offset(int value, char plus_minus)
lib.offset.argtypes = [ctypes.c_int, ctypes.c_char]
lib.offset.restype = ctypes.c_int

lib.loopback()

lib.multiply(5)

lib.offset(10, b'+')



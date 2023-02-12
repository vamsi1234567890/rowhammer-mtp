import ctypes
import pathlib
import numpy as np

if __name__ == "__main__":
	libname = pathlib.Path().absolute() / "libpa.so"
	c_lib = ctypes.CDLL(libname)
	arr = np.ones(100000000, dtype=np.int64)
	for i in range(len(arr)):
		arr[i]=-1
	arr = arr.astype(dtype=ctypes.c_ulong)
	arr_p=ctypes.POINTER(ctypes.c_ulong)
	arr_i = arr.ctypes.data_as(arr_p)
	c_lib.main(arr_i)
	

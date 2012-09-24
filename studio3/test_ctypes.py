from ctypes import *

sndio = cdll.LoadLibrary("libsndio.so")

MIO_OUT =   4
MIO_IN =    8

# mio_open(3)
sndio.mio_open.argtypes = [c_char_p, c_uint, c_int]
sndio.mio_open.restype = c_void_p

def mio_open(name, bio, flag):
    sndio.mio_open(name, bio, flag)

x = mio_open("rmidi/0", MIO_OUT, 0)

print(x)


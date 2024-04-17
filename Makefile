# define libraries
ifeq ($(shell uname),Darwin)
LIB = -DMAC -framework OPENGL -framework GLUT
else
LIB = -lGL -lGLU -lglut
endif
LIB2 = libim/libim.a jpeg/libjpeg.a

# define makefile targets
CC = g++ -std=c++11 -Wall -Wno-deprecated-declarations -O3

texture3: texture3.cpp $(LIB2)
	$(CC) -o main main.cpp $(LIB) $(LIB2)

jpeg/libjpeg.a:
	cd jpeg;make;cd ..

libim/libim.a:
	cd libim;make;cd ..


CC=g++
CXXFLAGS=-std=c++14 -Wall -O0 -ggdb3
EGL_LIBS=-lEGL -lGL
GL_LIBS=-lGL -lGLU -lGLEW
GLFW_LIBS=$(shell pkg-config --static --libs x11 xrandr xi xxf86vm glew glfw3)
LIBS=-lassimp -lGLEW -lpthread
INCS=-I/usr/include -Ilib

HDR=$(shell ls lib/*.h)
SRC=$(shell ls lib/*.cpp)
OBJ=$(patsubst %.cpp, %.o, $(SRC))

all: depth-render-egl depth-render-glfw $(HDR)

depth-render-egl: mainEGL.o $(OBJ)
	$(CC) $^ -o $@ $(LIBS) $(EGL_LIBS)

depth-render-glfw: mainGLFW.o $(OBJ)
	$(CC) $^ -o $@ $(LIBS) $(GL_LIBS) $(GLFW_LIBS)

%.o: %.c
	$(CC) -c $< -o $@ $(CXXFLAGS) $(INCS)

clean:
	rm -rf depth-render-* *.o lib/*.o

.PHONY: all clean

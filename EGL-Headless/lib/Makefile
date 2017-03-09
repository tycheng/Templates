CC=g++
CXXFLAGS=-std=c++14 -Wall -O0 -ggdb3
LIBS=-lEGL -lGLESv2 -lassimp -lGLEW -lpthread
INCS=-I/usr/include

SRC=$(shell ls *.cpp)
OBJ=$(patsubst %.cpp, %.o, $(SRC))

depth-render-egl: mainEGL.o $(OBJ)
	$(CC) $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@ $(CXXFLAGS) $(INCS)

clean:
	rm -rf depth-render-* *.o

.PHONY: clean

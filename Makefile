include Sources

LIBNOISE=$(SOURCES:.cpp=.o)

CFLAGS=-std=c++11 -I/usr/include/GLFW -I/usr/include -I/usr/include/glm
LDFLAGS= -L/usr/lib/x86_64-linux-gnu

all: landscape


clean:
	rm landscape $(LIBNOISE) landscape.o shader.o

landscape: landscape.o shader.o $(LIBNOISE)
	g++ $(LDFLAGS) -o $@  landscape.o shader.o $(LIBNOISE) -lglfw -lGLEW -lGL 


%.o: %.cpp
	g++ $(CFLAGS) -c $< -o $@

CIBLE = main
SRCS =  Main.cpp Camera.cpp Mesh.cpp GLProgram.cpp GLShader.cpp GLError.cpp LightSource.cpp Ray.cpp BVH.cpp
OPENGL_PATH = /usr/lib/nvidia # change this for your own environment
LIBS = -L$(OPENGL_PATH) -lglut -lGLU -lGL -lGLEW -lm -lpthread

CC = g++
CPP = g++

FLAGS = -Wall -g -pthread -O3 -std=c++11

CFLAGS = $(FLAGS)
CXXFLAGS = $(FLAGS)

OBJS = $(SRCS:.cpp=.o)

.PHONY : clean run %.off

$(CIBLE): $(OBJS)
	g++ $(LDFLAGS) -o $(CIBLE) $(OBJS) $(LIBS)

run : $(CIBLE)
	./$<

%.off : $(CIBLE)
	./$< models/$@

clean:
	rm -f  *~  $(CIBLE) $(OBJS)

Camera.o: Camera.cpp Camera.h Vec3.h
Mesh.o: Mesh.cpp Mesh.h Vec3.h
GLError.o: GLError.cpp GLError.h Exception.h
GLShader.o: GLShader.cpp GLShader.h GLError.h
GLProgram.o: GLProgram.cpp GLProgram.h GLShader.h GLError.h Exception.h
Main.o: Main.cpp Vec3.h Camera.h Mesh.h GLProgram.h Exception.h BoundingBox.h BVH.h
LightSource.o: LightSource.cpp LightSource.h
Ray.o: Ray.cpp Ray.h
BVH.o: BVH.h BVH.cpp BoundingBox.h

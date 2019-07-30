
GL_LIBS = `pkg-config --static --libs glfw3 glew` 
EXT = 
CPPFLAGS = `pkg-config --cflags glfw3`

CC = g++
EXE = assign2
OBJS = model-view.o Objects.o Shader.o Viewer.o Sphere.o

.PHONY: all clean

# If you haven't seen this before, notice how the rules
# of the Makefile build .o files from c++ source, and then
# build the executable from the .o files. Files are only
# re-made if they've been modified, or something they depend
# on has been modified.

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

model-view.o: model-view.cpp
	$(CC) $(CPPFLAGS) -c model-view.cpp

Shader.o : Shader.cpp Shader.hpp
	$(CC) $(CPPFLAGS) -c Shader.cpp

Viewer.o: Viewer.h Viewer.cpp
	$(CC) $(CPPFLAGS) -c Viewer.cpp

Sphere.o: Sphere.hpp Sphere.cpp	
	$(CC) $(CPPFLAGS) -c Sphere.cpp

Objects.o: Objects.h Objects.cpp	
	$(CC) $(CPPFLAGS) -c Objects.cpp

clean:
	rm -f *.o $(EXE)$(EXT)

EXENAME = bottom
OBJS = bottom.o main.o
CXX = g++
CXXFLAGS = `pkg-config --cflags opencv`
LD = g++
LDFLAGS = `pkg-config --libs opencv`
all: bottom
bottom: bottom.o
	$(LD) bottom.o $(LDFLAGS) -o bottom

clean : 
	-rm -f *.o bottom

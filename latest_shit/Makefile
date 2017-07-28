EXENAME = vision
OBJS = vision.o main.o
CXX = g++
CXXFLAGS = `pkg-config --cflags opencv`
LD = g++
LDFLAGS = `pkg-config --libs opencv`
all: vision
vision: vision.o main.o
	$(LD) vision.o main.o $(LDFLAGS) -o vision

clean : 
	-rm -f *.o vision

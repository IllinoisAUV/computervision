EXENAME = detect
OBJS = detect.o main.o
CXX = g++
CXXFLAGS = `pkg-config --cflags opencv`
LD = g++
LDFLAGS = `pkg-config --libs opencv`
all: detect
detect: detect.o main.o
	$(LD) detect.o main.o $(LDFLAGS) -o detect

clean : 
	-rm -f *.o detect

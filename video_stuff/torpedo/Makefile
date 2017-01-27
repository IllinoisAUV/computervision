EXENAME = torpedo
OBJS = torpedo.o main.o
CXX = g++
CXXFLAGS = `pkg-config --cflags opencv`
LD = g++
LDFLAGS = `pkg-config --libs opencv`
all: torpedo
torpedo: torpedo.o
	$(LD) torpedo.o $(LDFLAGS) -o torpedo

clean : 
	-rm -f *.o torpedo

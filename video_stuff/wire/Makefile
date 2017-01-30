EXENAME = wire
OBJS = wire.o main.o
CXX = g++
CXXFLAGS = `pkg-config --cflags opencv`
LD = g++
LDFLAGS = `pkg-config --libs opencv`
all: wire
wire: wire.o
	$(LD) wire.o $(LDFLAGS) -o wire

clean : 
	-rm -f *.o wire

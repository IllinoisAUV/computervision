EXENAME = video
OBJS = video.o main.o
CXX = g++
CXXFLAGS = `pkg-config --cflags opencv`
LD = g++
LDFLAGS = `pkg-config --libs opencv`
all: video
video: video.o
	$(LD) video.o $(LDFLAGS) -o video

clean : 
	-rm -f *.o video

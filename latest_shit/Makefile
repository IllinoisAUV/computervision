CC = g++
CFLAGS = -Wall -W -fopenmp -O3 -pedantic -std=c++0x
LFLAGS =
LLIBS =

ifdef FUNC
	OPTS = -DWHICH_FUNC=$(FUNC)
else
	OPTS = -DWHICH_FUNC=1
endif

all:
	make prog_TestAsyncRMP

clean:
	rm -f *.o
	rm -f prog_TestAsyncRMP
	rm -f prog_StereoMRF_*
	rm -f prog_TestRMP

prog_StereoMRF_$(FUNC): StereoMRF/StereoMRF.cpp libAsyncRMP/Region.h
	$(CC) StereoMRF/StereoMRF.cpp -o prog_StereoMRF_$(FUNC) $(CFLAGS) $(LFLAGS) $(LLIBS) $(OPTS)

prog_StereoMRF: prog_StereoMRF_$(FUNC)

prog_StereoMatching: StereoMatching/StereoMatching.cpp
	$(CC) StereoMatching/StereoMatching.cpp -o prog_StereoMatching $(CFLAGS) -IStereoMatching/ -lpng

prog_TestAsyncRMP: TestAsyncRMP/TestAsyncRMP.cpp libAsyncRMP/Region.h
	$(CC) TestAsyncRMP/TestAsyncRMP.cpp -o prog_TestAsyncRMP $(CFLAGS) $(LFLAGS) $(LLIBS) $(OPTS)

prog_TestRMP: TestRMP/TestRMP.cpp libAsyncRMP/Region.h
	$(CC) TestRMP/TestRMP.cpp -o prog_TestRMP $(CFLAGS) $(LFLAGS) $(LLIBS) $(OPTS)

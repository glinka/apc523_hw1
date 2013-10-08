toCompile =	main.o

CXX = gcc

CXXFLAGS = -g -Wall -I/usr/include/libpng -lm -lpng -std=c99 -fopenmp -O3

all: calc_set_omp

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $<

#%.o: %.h
#    $(CXX) $(CXXFLAGS) -c $<

calc_set_omp: $(toCompile)
	$(CXX) -o $@ $^ $(CXXFLAGS) 

clean:
	$(RM) *.o 

toCompile =	main.o

CXX = gcc

CXXFLAGS = -g -Wall -std=c99 -lm#-fopenmp #-O3

all: calc_set_omp

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $<

#%.o: %.h
#    $(CXX) $(CXXFLAGS) -c $<

calc_set_omp: $(toCompile)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	$(RM) *.o 

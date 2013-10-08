toCompile =	main.o

CXX = mpicc

CXXFLAGS = -g -Wall -I /usr/include/libpng -std=c99 -lm -lpng -O3

all: calc_set_mpi

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $<

#%.o: %.h
#    $(CXX) $(CXXFLAGS) -c $<

calc_set_mpi: $(toCompile)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	$(RM) *.o 

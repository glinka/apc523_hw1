toCompile =	main.o

CXX = mpicc

CXXFLAGS = -g -Wall -std=c99 -lm -O3

all: calc_set_mpi

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $<

#%.o: %.h
#    $(CXX) $(CXXFLAGS) -c $<

calc_set_mpi: $(toCompile)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	$(RM) *.o 

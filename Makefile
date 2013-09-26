toCompile = ValuedGrid.o

CXX = g++

CXXFLAGS = -g -Wall -std=c++0x #-O3

all: calc_set

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $<

#%.o: %.h
#    $(CXX) $(CXXFLAGS) -c $<

calc_set: $(toCompile)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	$(RM) *.o 

CXX = g++
INCLUDE = -I $(CURDIR)/include
CXXFLAGS = -std=c++11 -Wall

all: main.out

main.out: main.o file.o
	$(CXX) $(CXXFLAGS) -o $@ $^

file.o: src/file.cpp include/file.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

clean:
	rm -f *.o *.out

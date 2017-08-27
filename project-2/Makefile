CXX = g++
INCLUDE = -I $(CURDIR)/include
CXXFLAGS = -std=c++11 -Wall

all: main.out

main.out: main.o trie.o
	$(CXX) $(CXXFLAGS) -o $@ $^

trie.o: src/trie.cpp include/trie.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

clean:
	rm -f *.o *.out

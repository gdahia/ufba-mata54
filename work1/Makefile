CXX = g++
INCLUDE = -I $(CURDIR)/include
CXXFLAGS = -std=c++11

all: main.out

main.out: main.o file.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: src/%.cpp include/%.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

clean:
	rm -f *.o *.out

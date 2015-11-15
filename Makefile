CXX=clang++
CXXFLAGS=--std=c++11 -O1

# put path to your LOKI library here
INCS=

all: ut pt

ut: unit.test.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

pt: perf.test.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -DNDEBUG $< -o $@

unittest: ut
	./ut

perftest: pt
	./pt

clean:
	rm -rf ut pt


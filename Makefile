CXXFLAGS += -O3 -ffast-math -std=c++20
snake: main.o
	$(CXX) $(CXXFLAGS) -o snake main.o -lraylib

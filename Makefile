CXXFLAGS += -O3 -fomit-frame-pointer -ffast-math -std=c++20
LDLIBS += -lraylib

snake: main.o
	$(CXX) -o snake main.o $(LDLIBS)
	strip -s snake

clean:
	rm snake
	rm *.o

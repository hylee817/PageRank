CC = g++
CXXFLAGS = -O3 -fopenmp

test : test.o LLAMA.o
	$(CC) $(CXXFLAGS) -o test test.o LLAMA.o

test.o : test.cpp vertex.h
	$(CC) $(CXXFLAGS) -c test.cpp vertex.h

LLAMA.o : LLAMA.cpp LLAMA.h
	$(CC) $(CXXFLAGS) -c LLAMA.cpp

clean : 
	rm *.o test

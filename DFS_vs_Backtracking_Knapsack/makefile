FLAGS = -Wall -std=c++11 -g 

all:  dfs

dfs: DFSprog.o
	g++ $(FLAGS) DFSprog.o -o dfs

DFSprog.o: DFSprog.cpp
	g++ $(FLAGS) -c DFSprog.cpp -o DFSprog.o

clean:
	rm -f *.o dfs

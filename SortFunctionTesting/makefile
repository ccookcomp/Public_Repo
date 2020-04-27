
# Here's the flag for C++11 support and a flag asking to be warned about
# everything. 
FLAGS = -Wall -std=c++11

all: Lab5 Lab5T

Lab5: Lab5.o Sorter.o 
	g++ $(FLAGS) Lab5.o Sorter.o -o Lab5

Sorter.o: Sorter.cpp Sorter.h
	g++ $(FLAGS) -c Sorter.cpp -o Sorter.o

Lab5.o: Lab5.cpp Sorter.h
	g++ $(FLAGS) -c Lab5.cpp -o Lab5.o

Lab5T: Lab5T.o TSorter.o Rectangle.o
	g++ $(FLAGS) Lab5T.o TSorter.o Rectangle.o -o Lab5T

TSorter.o: TSorter.cpp TSorter.h
	g++ $(FLAGS) -c TSorter.cpp -o TSorter.o

Rectangle.o: Rectangle.cpp Rectangle.h
	g++ $(FLAGS) -c Rectangle.cpp -o Rectangle.o

Lab5T.o: Lab5T.cpp TSorter.h
	g++ $(FLAGS) -c Lab5T.cpp -o Lab5T.o


clean:
	rm -f *.o Lab5
	rm -f *.o Lab5T


#include <iostream>
#include <cstdlib>

using namespace std;

class Sorter {
   private:
      int *array; 
      unsigned int numElements;
      unsigned int capacity;

      void swap(int i, int j); 

  public:
      Sorter();
      Sorter(unsigned int initialCapacity);
      ~Sorter();

      void clear(); 
      bool insert(int value); 

      // Make sure the array contains at least newCapacity elements.
      // If not, grow it to that size and copy in old values
      unsigned int makeSpace(unsigned int newCapacity); 

      // Show the first n elements, k per line using << 
      void show(unsigned int n, unsigned int k); 

      // "Shuffle" the array elements
      void shuffle(); 

      // These are the functions you should implement for Lab 5
      // You should keep these interfaces the same, but you may add
      // other "helper" functions if necessary.
      void insertionSortI();
      void selectionSortI();
      void bubbleSortI();     
      void insertionSortR();
      void insertionSortR3(unsigned int i,unsigned int num,int num2);
      void selectionSortR();
      void selectionSortR2(unsigned int i,unsigned int j,unsigned int min);	
      void bubbleSortR(); 
      void bubbleSortR2(unsigned int i, unsigned int j);	
};


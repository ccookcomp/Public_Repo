#ifndef TSORTER_H
#define TSORTER_H
#include <iostream>
using namespace std;

#include <cstdlib>
#include <vector>
#include <stack>
#include "Rectangle.h"

template <class T> class TSorter {
   private:
	typename vector<T>::iterator it;
      vector <T> myVector;
      void swap(typename vector<T>::iterator i, typename vector<T>::iterator j);
	unsigned int numElements; 

  public:
      TSorter();
      ~TSorter();

      void clear(); 
      bool insert(T value); 

      // Make sure the array contains at least newCapacity elements.
      // If not, grow it to that size and copy in old values
      //unsigned int makeSpace(unsigned int newCapacity); 

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
      void insertionSortR3(typename vector<T>::iterator i,typename vector<T>::iterator place,T num2,unsigned int stop);
      void selectionSortR();
      void selectionSortR2(typename vector<T>::iterator i,typename vector<T>::iterator j,typename vector<T>::iterator min);	
      void bubbleSortR(); 
      void bubbleSortR2(typename vector<T>::iterator i, typename vector<T>::iterator j);	
};

#endif

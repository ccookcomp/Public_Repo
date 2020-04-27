#include <iostream>
using namespace std;

#include "Sorter.h"

//Function Given by Professor
void Sorter::swap(int i, int j) {
   int tmp; 
   tmp = array[i]; 
   array[i] = array[j];
   array[j] = tmp; 
   return; 
}

Sorter::Sorter() {
   numElements = 0;
   capacity = 0;
   array = NULL;  
}

Sorter::Sorter(unsigned int initialCapacity) {
   capacity = initialCapacity; 
   numElements = 0;
   array = new int[capacity]; 
}

unsigned int Sorter::makeSpace(unsigned int newCapacity) {
   int *old; 
   unsigned int i;
   if (newCapacity > capacity) {
      old = array; 
      array = new int[newCapacity]; 
      for (i=0; i<numElements; i++) 
         array[i] = old[i]; 
      capacity = newCapacity;
      delete []old; 
   }

   return capacity;
}

void Sorter::clear() {
   numElements = 0;
   return; 
}

bool Sorter::insert(int element) {
   if (numElements >= capacity) 
      return false;
   array[numElements++] = element;  
   return true;
}

Sorter::~Sorter() {
   if (array) {
      delete []array; 
      numElements = 0; 
   }
}

// Shuffle array elements
//Function Given by Professor
void Sorter::shuffle() {
   unsigned int j; 
   for (unsigned int i=1; i<numElements; i++) {
      j = rand() % i; 
      swap(i, j); 
   }
}

// Show the first n elements, k per line, using << 
//Function Given by Professor
void Sorter::show(unsigned int n, unsigned int k) {
   unsigned int toShow = ((n < numElements) ? n : capacity); 
   for (unsigned int i=0; i < toShow; i++) {
      if (!(i%k)) cout << endl;
      cout << array[i] << " "; 
   }
   cout << endl << endl;
} 

// Iterative Insertion Sort
void Sorter::insertionSortI() {
	int num;
	unsigned int num2;
	for (unsigned int i=1;i<numElements;i++){
		num=array[i];
		num2=i;
		while((num2>0)&&(array[num2-1]>num)){
			array[num2]=array[num2-1];
			num2--;
			}
		array[num2]=num;		
		}	
}

// Iterative Selection Sort
void Sorter::selectionSortI() {
	int num;
	for (unsigned int i=0;i<numElements-1;i++){
		num=i;
		for (unsigned int j=i+1;j<numElements; j++){
			if(array[j]<array[num]){num=j;}
			}
		swap(i,num);
		}
}

void Sorter::selectionSortR2(unsigned int i,unsigned int j,unsigned int min) {
	if(i<numElements-1){
		if(j<numElements){
			if(array[j]<array[min]){selectionSortR2(i,j+1,j);}
			else{selectionSortR2(i,j+1,min);}
		}else{
			swap(i,min);
			selectionSortR2(i+1,i+2,min);
			}
		}
			

}

// Recursive Selection Sort
void Sorter::selectionSortR() {
   selectionSortR2(0,1,0);
}

// Iterative Bubble Sort
void Sorter::bubbleSortI() {
   for (unsigned int i=numElements-1; i>=1; i--){
	for (unsigned int j=1; j<=i; j++){
		if(array[j-1]>array[j]){swap(j-1,j);}
		}
	}
}	

void Sorter::bubbleSortR2(unsigned int i, unsigned int j) {
	if(i>=1){
		if(j<=i){
			if(array[j-1]>array[j]){
			swap(j-1,j);}
			bubbleSortR2(i,j+1);
		}else{bubbleSortR2(i-1,1);}
	}
}

// Recursive Bubble Sort
void Sorter::bubbleSortR() {
   bubbleSortR2(numElements-1,1);
}

void Sorter::insertionSortR3(unsigned int i,unsigned int num,int num2){
	if((i>0)&&(array[i-1]>num2)){
		array[i]=array[i-1];
		insertionSortR3(i-1,num,num2);
		}
	else{
		array[i]=num2;
		if(num<numElements){
			insertionSortR3(num+1,num+1,array[num+1]);
		}
	}
	
}


     
// Recursive Insertion Sort
void Sorter::insertionSortR() {
	insertionSortR3(1,1,array[1]);
}
 

	



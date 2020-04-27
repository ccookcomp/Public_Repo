#include <iostream>
using namespace std;

#include "TSorter.h"

//Function Given by Professor
template<class T> void TSorter<T>::swap(typename vector<T>::iterator i, typename vector<T>::iterator j) {
   T tmp;
   tmp = *i; 
   *i = *j;
   *j = tmp; 
   return; 
}

template<class T> TSorter<T>::TSorter() {
 numElements = myVector.size();
}

template<class T> void TSorter<T>::clear() {
   myVector.clear();
	numElements=0;
   return; 
}

template<class T> bool TSorter<T>::insert(T element) {
    myVector.push_back(element); 
    numElements++; 
   return true;
}

template<class T> TSorter<T>::~TSorter() {
   //if (myVector) {
      //delete myVector; //Local Vector. No free/delete needed
      //numElements = 0; 
  // }
}

// Shuffle myVector elements
//Function Given by Professor
template<class T> void TSorter<T>::shuffle() {
   unsigned int j; 
	typename vector<T>::iterator i1=myVector.begin();
	typename vector<T>::iterator j1=myVector.begin();
   for (unsigned int i=1; i<numElements; i++) {
      j = rand() % i; 
	typename vector<T>::iterator i1=myVector.begin();
	typename vector<T>::iterator j1=myVector.begin();
	for (unsigned int x=0;x<i;x++){i1++;}
	for (unsigned int x=0;x<j;x++){j1++;}
      swap(i1, j1); 
   }
}

// Show the first n elements, k per line, using << 
//Function Given by Professor
template<class T> void TSorter<T>::show(unsigned int n, unsigned int k) {
	typename vector<T>::iterator i1=myVector.begin();
	for (unsigned int x=0; x<myVector.size();x++){
		cout<<*i1<<" ";
		i1++;
	}
   cout << endl << endl;
} 

// Iterative Snsertion Sort

// Iterative Selection Sort
template<class T> void TSorter<T>::selectionSortI() {
	typename vector<T>::iterator num;
	typename vector<T>::iterator i=myVector.begin();
	typename vector<T>::iterator j=myVector.begin();
	for (unsigned int x=0;x<numElements-1;x++){
		i=myVector.begin();
		for (unsigned int y=0;y<x;y++){i++;}
		num=i;
		for (unsigned int z=x+1;z<numElements; z++){
			j=myVector.begin();
			for (unsigned int y=0;y<z;y++){j++;}
			if(*j<*num){num=j;}
			}
		swap(i,num);
		}
}

template<class T> void TSorter<T>::selectionSortR2(typename vector<T>::iterator i,typename vector<T>::iterator j,typename vector<T>::iterator min) {
	typename vector<T>::iterator x=myVector.end();
	typename vector<T>::iterator y=myVector.end();
	if(i!=x){
		if(j!=y){
			if(*(j)<*(min)){
				min=j;
				j++;
				selectionSortR2(i,j,min);					
				}
			else{
				j++;
				selectionSortR2(i,j,min);}
		}else{
			swap(i,min);
			i++;
			j=i;
			j++;
			min=i;
			selectionSortR2(i,j,min);
			}
		}
			

}

// Recursive Selection Sort
template<class T> void TSorter<T>::selectionSortR() {
	typename vector<T>::iterator i=myVector.begin();
	typename vector<T>::iterator j=i;
	j++;
   selectionSortR2(i,j,i);
}

// Iterative Bubble Sort
template<class T> void TSorter<T>::bubbleSortI() {
	typename vector<T>::iterator num;
	typename vector<T>::iterator i=myVector.begin();
	typename vector<T>::iterator j=myVector.begin();
   for (unsigned int x=numElements-1; x>=1; x--){
	i=myVector.begin();
	for (unsigned int y=0;y<x;y++){i++;}
	for (unsigned int z=1; z<=x; z++){
		j=myVector.begin();
		for (unsigned int y=0;y<z;y++){j++;}
		num=j;
		num--;
		if(*num>*j){swap(num,j);}
		}
	}
}	

template<class T> void TSorter<T>::bubbleSortR2(typename vector<T>::iterator i, typename vector<T>::iterator j) {
	if(i!=myVector.begin()){
		if(j<=i){
			it=j;
			it--;
			if(*it>*j){
			swap(it,j);}
			j++;
			bubbleSortR2(i,j);
		}else{
			i--;
			j=myVector.begin();
			j++;
			bubbleSortR2(i,j);}
	}
}

// Recursive Bubble Sort
template<class T> void TSorter<T>::bubbleSortR() {
	typename vector<T>::iterator i=myVector.end();
	i--;
	typename vector<T>::iterator j=myVector.begin();
	j++;
   bubbleSortR2(i,j);
}
//Iterative Insertion Sort
template<class T> void TSorter<T>::insertionSortI() {
	T num;
	typename vector<T>::iterator num2;
	typename vector<T>::iterator i=myVector.begin();
	for (unsigned int x=1;x<numElements;x++){
		i=myVector.begin();
		for (unsigned int y=0;y<x;y++){i++;}
		num=*i;
		num2=i;
		num2--;
		while((i!=myVector.begin())&&(*(num2)>num)){
			swap(i,num2);
			i--;
			num2--;
			}
		*i = num;		
		}	
}


//Recursive Insertion Sort
template<class T> void TSorter<T>::insertionSortR3(typename vector<T>::iterator i,typename vector<T>::iterator place,T num2,unsigned int stop){
	it=i;
	if (i!=myVector.begin()){it--;}
	if((i!=myVector.begin())&&(*(it)>num2)){
		swap(i,it);
		i--;
		insertionSortR3(i,place,num2,stop);
		}
	else{
		*i=num2;
		if(stop<numElements-1){
			place++;
			stop++;
			insertionSortR3(place,place,*place,stop);
		}
	}
	
}


     
// Recursive Insertion Sort
template<class T> void TSorter<T>::insertionSortR() {
	typename vector<T>::iterator i=myVector.begin();
	i++;
	insertionSortR3(i,i,*i,1);
}

template class TSorter<int>;
template class TSorter<string>;
template class TSorter<Rectangle>;


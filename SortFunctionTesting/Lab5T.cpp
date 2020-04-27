#include <iostream>
using namespace std;
#include <string>
#include <cstdlib>

#include "TSorter.h"
//#include "Rectangle.h"

#define DEFAULT_NUMVALUEs 10
#define MAX_NUMVALUEs 1000
#define MIN_NUMVALUEs 1

int main (int argc, char **argv) {
   unsigned int numValues = DEFAULT_NUMVALUEs;//Assigns numValues to a default
   int value;//int used to add random numbers to be sorted

   if (argc > 1)
      numValues = atoi(argv[1]);//reassigns numValues if specified by user
	//Handling case where the argument is out of range
   if ((numValues < MIN_NUMVALUEs) || (numValues > MAX_NUMVALUEs)) {
      cerr << "specify numValues in the range [" << MIN_NUMVALUEs; 
      cerr << ", " << MAX_NUMVALUEs <<"]" << endl; 
      cerr << "\nUsage: " << argv[0] << " {numValues}" << endl; 
	  exit(0);
   }
	//Handling case where arguments are more than 2
   if (argc > 2) 
      cerr << "Ignoring extra command line arguments." << endl; 

   cout << endl << "Generating " << numValues << " values for sorting." << endl; 
   TSorter<int> s;//Declares an instance of class TSorter for template int

	srand(time(NULL)); // use this line for pseudo-random numbers 

   for (unsigned int i=0; i < numValues; i++) {
      value = rand() % 100;  //Assigns random numbers from 0-100 to be sorted
      s.insert(value); //Inserts random values
   } 
	TSorter<string> s1;//Declares an instance of class TSorter for template string
	//Inserts strings to be sorted
	s1.insert("Hello");
	s1.insert("Chicken");
	s1.insert("Tastes");
	s1.insert("Really");
	s1.insert("Good");
	s1.insert("When");
	s1.insert("Eaten");
	s1.insert("Hot");
	
	TSorter<Rectangle> r1;//Declares an instance of class TSorter for template rectangle
	//Inserts Rectangle classes to be sorted
	r1.insert(Rectangle(5,10));
	r1.insert(Rectangle(5,15));
	r1.insert(Rectangle(2,10));
	r1.insert(Rectangle(5,9));
	r1.insert(Rectangle(3,7));
	r1.insert(Rectangle(5,11));
	r1.show(25,10);

   cout << "Initial array: \n";
   s.show(25, 10); 

   s.insertionSortI();
   cout << "After iterative Insertion sort:\n";
   s.show(25, 10);
	

   s.shuffle(); 
   cout << "shuffled: \n"; 
   s.show(25, 10); 

   s.selectionSortI();
   cout << "After iterative selection sort:\n";
   s.show(25, 10); 

   s.shuffle(); 
   cout << "shuffled: \n"; 
   s.show(25, 10); 

   s.bubbleSortI();
   cout << "After iterative Bubble sort:\n";
   s.show(25, 10); 

   s.shuffle(); 
   cout << "shuffled: \n"; 
   s.show(25, 10); 

   s.insertionSortR();
   cout << "After recursive Insertion sort:\n";
   s.show(25, 10); 

   s.shuffle(); 
   cout << "shuffled: \n"; 
   s.show(25, 10); 

   s.selectionSortR();
   cout << "After recursive selection sort:\n";
   s.show(25, 10); 

   s.shuffle(); 
   cout << "shuffled: \n"; 
   s.show(25, 10); 

   s.bubbleSortR();
   cout << "After recursive Bubble sort:\n";
   s.show(25, 10); 

   s.shuffle(); 
   cout << "shuffled: \n"; 
   s.show(25, 10); 
//String
cout << "Initial array: \n";
   s1.show(25, 10); 

   s1.insertionSortI();
   cout << "After iterative Insertion sort:\n";
   s1.show(25, 10);
	

   s1.shuffle(); 
   cout << "shuffled: \n"; 
   s1.show(25, 10); 

   s1.selectionSortI();
   cout << "After iterative selection sort:\n";
   s1.show(25, 10); 

   s1.shuffle(); 
   cout << "shuffled: \n"; 
   s1.show(25, 10); 

   s1.bubbleSortI();
   cout << "After iterative Bubble sort:\n";
   s1.show(25, 10); 

   s1.shuffle(); 
   cout << "shuffled: \n"; 
   s1.show(25, 10); 

   s1.insertionSortR();
   cout << "After recursive Insertion sort:\n";
   s1.show(25, 10); 

   s1.shuffle(); 
   cout << "shuffled: \n"; 
   s1.show(25, 10); 

   s1.selectionSortR();
   cout << "After recursive selection sort:\n";
   s1.show(25, 10); 

   s1.shuffle(); 
   cout << "shuffled: \n"; 
   s1.show(25, 10); 

   s1.bubbleSortR();
   cout << "After recursive Bubble sort:\n";
   s1.show(25, 10); 

   s1.shuffle(); 
   cout << "shuffled: \n"; 
   s1.show(25, 10);
//Rectangle
cout << "Initial array: \n";
   r1.show(25, 10); 

   r1.insertionSortI();
   cout << "After iterative Insertion sort:\n";
   r1.show(25, 10);
	

   r1.shuffle(); 
   cout << "shuffled: \n"; 
   r1.show(25, 10); 

   r1.selectionSortI();
   cout << "After iterative selection sort:\n";
   r1.show(25, 10); 

   r1.shuffle(); 
   cout << "shuffled: \n"; 
   r1.show(25, 10); 

   r1.bubbleSortI();
   cout << "After iterative Bubble sort:\n";
   r1.show(25, 10); 

   r1.shuffle(); 
   cout << "shuffled: \n"; 
   r1.show(25, 10); 

   r1.insertionSortR();
   cout << "After recursive Insertion sort:\n";
   r1.show(25, 10); 

   r1.shuffle(); 
   cout << "shuffled: \n"; 
   r1.show(25, 10); 

   r1.selectionSortR();
   cout << "After recursive selection sort:\n";
   r1.show(25, 10); 

   r1.shuffle(); 
   cout << "shuffled: \n"; 
   r1.show(25, 10); 

   r1.bubbleSortR();
   cout << "After recursive Bubble sort:\n";
   r1.show(25, 10); 

   r1.shuffle(); 
   cout << "shuffled: \n"; 
   r1.show(25, 10); 
}


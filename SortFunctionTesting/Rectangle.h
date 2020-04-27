#include <iostream>
using namespace std;

class Rectangle {
	private:
	int length;
	int width;
	int area;
	public:
	Rectangle(int length,int width);
	int getArea();
	Rectangle();
	friend ostream & operator<<(ostream &out, const Rectangle &rect);
	bool operator<(const Rectangle& source);
	bool operator>(const Rectangle& source);
};

#include <iostream>
using namespace std;

#include "Rectangle.h"

Rectangle::Rectangle(int w,int l){
	area=l*w;
	}
Rectangle::Rectangle(){
	area=0;
}
int Rectangle::getArea(){
	return area;
	}
ostream & operator<<(ostream &out, const Rectangle &rect){
		out<<rect.area;
		return out;
	}
bool Rectangle::operator<(const Rectangle& source){
		if (this->getArea()<source.area){return true;}
		else{return false;}
		}
bool Rectangle::operator>(const Rectangle& source){
		if (this->getArea()>source.area){return true;}
		else{return false;}
		}

#include "ScrollingVector.h"

void ScrollingVector::addData(float x, float y, int maxSize){
	if (this->data.size() < maxSize) {
		this->data.push_back(ImVec2(x, y));
	}
	else if (this->pointer == maxSize) {
		this->pointer = 0;
		this->data[this->pointer].x = x;
		this->data[this->pointer].y = y;
	}
	else {
		this->data[this->pointer].x = x;
		this->data[this->pointer].y = y;
		this->pointer++;
	}
}

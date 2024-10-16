#pragma once
#include "ScrollingVector.h"
#include "Common.h"

struct Data {
	std::string label;
	ScrollingVector scrVec;
	bool locked = false;
};
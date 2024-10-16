#pragma once
#include "Common.h"
struct ScrollingVector {
public:
	ImVector<ImVec2> data;
	int pointer = 0;

	void addData(float x, float y, int maxSize);
};
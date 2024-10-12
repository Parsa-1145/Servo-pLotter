#include "Common.h"

float getLength(vec2 vector) {
	return hypot(vector[0], vector[1]);
}

float distFromLine(vec2 p, vec2 l1, vec2 l2) {
	float lengths[3] = { getLength(p - l1), getLength(p - l2), getLength(l1 - l2) };
	float c = (lengths[0] + lengths[1] + lengths[2]) / 2;
	float a = 2 * sqrt(c * (c - lengths[0]) * (c - lengths[1]) * (c - lengths[2])) / lengths[2];
	return a;
}
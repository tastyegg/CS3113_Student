#pragma once
#include <math.h>

class Vector {
public:
	Vector();

	void normalize();
	float length();
	float x, y, z;
};
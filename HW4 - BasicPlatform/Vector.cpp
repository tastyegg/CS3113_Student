#include "Vector2.h"

Vector::Vector()
	: x(0), y(0), z(0) {

}

float Vector::length() {
	return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
}

void Vector::normalize() {
	float len = length();

	x = x / len;
	y = y / len;
	z = z / len;
}
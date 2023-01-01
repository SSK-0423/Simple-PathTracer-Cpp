#pragma once
#include "Vector.hpp"
#include "Matrix.hpp"

#define PI 3.14159265359f

inline float Min(float rhs, float lhs) {
	if (rhs > lhs) return lhs;
	if (rhs < lhs) return rhs;
}
#pragma once
constexpr float PI = 3.14159265359f;
constexpr float EPSILON = 0.0001f;

inline float Max(float rhs, float lhs) {
	if (rhs > lhs) return rhs;
	else if (lhs > rhs) return lhs;
	else return rhs;
}

inline float Min(float rhs, float lhs) {
	if (rhs < lhs) return rhs;
	else if (lhs < rhs) return lhs;
	else return rhs;
}

inline float Saturate(float num) {
	return Min(Max(num, 0.f), 1.f);
}
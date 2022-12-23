#pragma once
#include <cmath>

struct Vector3 {
	Vector3() : r(0), g(0), b(0) {};
	Vector3(float r, float g, float b) : r(r), g(g), b(b) {}

	float r;
	float g;
	float b;

	Vector3 operator +(Vector3 vec) {
		Vector3 ret;
		ret.r = this->r + vec.r;
		ret.g = this->g + vec.g;
		ret.b = this->b + vec.b;
		return ret;
	}

	Vector3 operator +=(Vector3 vec) {
		this->r += vec.r;
		this->g += vec.g;
		this->b += vec.b;

		return *this;
	}

	Vector3 operator -(Vector3 vec) {
		Vector3 ret;
		ret.r = this->r - vec.r;
		ret.g = this->g - vec.g;
		ret.b = this->b - vec.b;
		return ret;
	}

	Vector3 operator -=(Vector3 vec) {
		this->r -= vec.r;
		this->g -= vec.g;
		this->b -= vec.b;

		return *this;
	}

	Vector3 operator +(float value) {
		Vector3 ret;
		ret.r = this->r + value;
		ret.g = this->g + value;
		ret.b = this->b + value;
		return ret;
	}

	Vector3 operator -(float value) {
		Vector3 ret;
		ret.r = this->r - value;
		ret.g = this->g - value;
		ret.b = this->b - value;
		return ret;
	}

	Vector3 operator *(float value) {
		Vector3 ret;
		ret.r = this->r * value;
		ret.g = this->g * value;
		ret.b = this->b * value;
		return ret;
	}

	Vector3 operator /(float value) {
		Vector3 ret;
		ret.r = this->r / value;
		ret.g = this->g / value;
		ret.b = this->b / value;
		return ret;
	}

	float length() {
		return std::sqrtf(r * r + g * g + b * b);
	}
};

inline float Dot(Vector3 rhs, Vector3 lhs) {
	return rhs.r * lhs.r + rhs.g * lhs.g + rhs.b * lhs.b;
}

inline Vector3 normalize(Vector3 vec) {
	return Vector3(vec.r / vec.length(), vec.g / vec.length(), vec.b / vec.length());
}

inline Vector3 Saturate(Vector3 vec) {
	if (vec.r < 0.f) { vec.r = 0.f; }
	else if (vec.r > 1.f) { vec.r = 1.f; }

	if (vec.g < 0.f) { vec.g = 0.f; }
	else if (vec.g > 1.f) { vec.g = 1.f; }

	if (vec.b < 0.f) { vec.b = 0.f; }
	else if (vec.b > 1.f) { vec.b = 1.f; }

	return vec;
}
#pragma once
#include <cmath>
#include "MathUtility.hpp"

struct Vector3 {
	Vector3() : x(0), y(0), z(0) {};
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	float x;
	float y;
	float z;

	Vector3 operator +(const Vector3 vec) const {
		Vector3 ret;
		ret.x = this->x + vec.x;
		ret.y = this->y + vec.y;
		ret.z = this->z + vec.z;
		return ret;
	}

	Vector3 operator +=(const Vector3 vec) {
		this->x += vec.x;
		this->y += vec.y;
		this->z += vec.z;

		return *this;
	}

	Vector3 operator -(const Vector3 vec) const {
		Vector3 ret;
		ret.x = this->x - vec.x;
		ret.y = this->y - vec.y;
		ret.z = this->z - vec.z;
		return ret;
	}

	Vector3 operator -=(Vector3 vec) {
		this->x -= vec.x;
		this->y -= vec.y;
		this->z -= vec.z;

		return *this;
	}

	Vector3 operator +(const float value) const {
		Vector3 ret;
		ret.x = this->x + value;
		ret.y = this->y + value;
		ret.z = this->z + value;
		return ret;
	}

	Vector3 operator -(const float value) const {
		Vector3 ret;
		ret.x = this->x - value;
		ret.y = this->y - value;
		ret.z = this->z - value;
		return ret;
	}

	Vector3 operator *(const Vector3 vec) const {
		Vector3 ret;
		ret.x = this->x * vec.x;
		ret.y = this->y * vec.y;
		ret.z = this->z * vec.z;
		return ret;
	}

	friend Vector3 operator *(const float value, const Vector3 vec) {
		Vector3 ret;
		ret.x = vec.x * value;
		ret.y = vec.y * value;
		ret.z = vec.z * value;
		return ret;

	}

	Vector3 operator *(const float value) const {
		Vector3 ret;
		ret.x = this->x * value;
		ret.y = this->y * value;
		ret.z = this->z * value;
		return ret;
	}

	Vector3 operator *=(const float value) {
		this->x *= value;
		this->y *= value;
		this->z *= value;

		return *this;
	}

	Vector3 operator /(const float value) const {
		Vector3 ret;
		ret.x = this->x / value;
		ret.y = this->y / value;
		ret.z = this->z / value;
		return ret;
	}

	float r() const { return x; }
	float g() const { return y; }
	float b() const { return z; }

	float Length() const {
		return std::sqrtf(x * x + y * y + z * z);
	}
};

inline float Dot(Vector3 rhs, Vector3 lhs) {
	return rhs.x * lhs.x + rhs.y * lhs.y + rhs.z * lhs.z;
}

inline Vector3 Cross(Vector3 rhs, Vector3 lhs) {
	float x = rhs.y * lhs.z - rhs.z * lhs.y;
	float y = rhs.z * lhs.x - rhs.x * lhs.z;
	float z = rhs.x * lhs.y - rhs.y * lhs.x;

	return Vector3(x, y, z);
}

inline Vector3 Normalize(Vector3 vec) {
	return Vector3(vec.x / vec.Length(), vec.y / vec.Length(), vec.z / vec.Length());
}

inline Vector3 Saturate(Vector3 vec) {
	return Vector3(Saturate(vec.x), Saturate(vec.y), Saturate(vec.z));
}
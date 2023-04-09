#pragma once
#include <cmath>
#include <stdio.h>
#include <cassert>
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

	Vector3 operator *=(const Vector3 vec) {
		this->x *= vec.x;
		this->y *= vec.y;
		this->z *= vec.z;

		return *this;
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

	Vector3 operator /=(const float value) {
		this->x /= value;
		this->y /= value;
		this->z /= value;

		return *this;
	}

	bool operator ==(const Vector3 vec) const {
		if (this->x == vec.x || this->y == vec.y || this->z == vec.z) { return true; }
		return false;
	}

	bool operator !=(const Vector3 vec) const {
		if (this->x != vec.x || this->y != vec.y || this->z != vec.z) { return true; }
		return false;
	}

	float operator [](size_t index) const {
		assert(index < 3 && index >= 0);
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
	}
	float& operator [](size_t index) {
		assert(index < 3 && index >= 0);
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
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

struct Vector4 {
	Vector4() : x(0), y(0), z(0), w(0) {};
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vector4(const Vector3& vec, float w) :x(vec.x), y(vec.y), z(vec.z), w(w) {}

	float x;
	float y;
	float z;
	float w;

	Vector4 operator +(const Vector4 vec) const {
		Vector4 ret;
		ret.x = this->x + vec.x;
		ret.y = this->y + vec.y;
		ret.z = this->z + vec.z;
		ret.w = this->w + vec.w;
		return ret;
	}

	Vector4 operator +=(const Vector4 vec) {
		this->x += vec.x;
		this->y += vec.y;
		this->z += vec.z;
		this->w += vec.w;

		return *this;
	}

	Vector4 operator -(const Vector4 vec) const {
		Vector4 ret;
		ret.x = this->x - vec.x;
		ret.y = this->y - vec.y;
		ret.z = this->z - vec.z;
		ret.w = this->w - vec.w;
		return ret;
	}

	Vector4 operator -=(Vector4 vec) {
		this->x -= vec.x;
		this->y -= vec.y;
		this->z -= vec.z;
		this->w -= vec.w;

		return *this;
	}

	Vector4 operator +(const float value) const {
		Vector4 ret;
		ret.x = this->x + value;
		ret.y = this->y + value;
		ret.z = this->z + value;
		ret.w = this->w + value;
		return ret;
	}

	Vector4 operator -(const float value) const {
		Vector4 ret;
		ret.x = this->x - value;
		ret.y = this->y - value;
		ret.z = this->z - value;
		ret.w = this->w - value;
		return ret;
	}

	Vector4 operator *(const Vector4 vec) const {
		Vector4 ret;
		ret.x = this->x * vec.x;
		ret.y = this->y * vec.y;
		ret.z = this->z * vec.z;
		ret.w = this->w * vec.z;
		return ret;
	}

	friend Vector4 operator *(const float value, const Vector4 vec) {
		Vector4 ret;
		ret.x = vec.x * value;
		ret.y = vec.y * value;
		ret.z = vec.z * value;
		ret.w = vec.w * value;
		return ret;

	}

	Vector4 operator *(const float value) const {
		Vector4 ret;
		ret.x = this->x * value;
		ret.y = this->y * value;
		ret.z = this->z * value;
		ret.w = this->w * value;
		return ret;
	}

	Vector4 operator *=(const float value) {
		this->x *= value;
		this->y *= value;
		this->z *= value;
		this->w *= value;

		return *this;
	}

	Vector4 operator /(const float value) const {
		Vector4 ret;
		ret.x = this->x / value;
		ret.y = this->y / value;
		ret.z = this->z / value;
		ret.w = this->w / value;
		return ret;
	}

	float operator [](const size_t index) const {
		assert(index < 4 && index >= 0);

		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		}
	}

	float& operator [](const size_t index) {
		assert(index < 4 && index >= 0);

		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		}
	}

	float r() const { return x; }
	float g() const { return y; }
	float b() const { return z; }
	float a() const { return w; }

	Vector3 xyz() const { return Vector3(x, y, z); }

	float Length() const {
		return std::sqrtf(x * x + y * y + z * z + w * w);
	}

	void Show() const {
		printf("[%6.2f %6.2f %6.2f %6.2f]\n", x, y, z, w);
	}
};

inline float Dot(Vector4 rhs, Vector4 lhs) {
	return rhs.x * lhs.x + rhs.y * lhs.y + rhs.z * lhs.z + rhs.w * lhs.w;
}

inline Vector4 Normalize(Vector4 vec) {
	float length = vec.Length();
	return Vector4(vec.x / length, vec.y / length, vec.z / length, vec.w / length);
}

inline Vector4 Saturate(Vector4 vec) {
	return Vector4(Saturate(vec.x), Saturate(vec.y), Saturate(vec.z), Saturate(vec.w));
}
#pragma once
#include <cmath>
#include "Vector.hpp"

struct Quaternion {
	Quaternion() : m_vec(0, 0, 0), w(0.f) {}
	Quaternion(const Vector3& vec, const float& w) : m_vec(vec), w(w) {}
	Quaternion(const float& x, const float& y, const float& z, const float& w)
		: m_vec(x, y, z), w(w) {}
	~Quaternion() = default;

	Vector3 m_vec;
	float w;

	float x() const { return m_vec.x; }
	float y() const { return m_vec.y; }
	float z() const { return m_vec.z; }

	Quaternion operator*(const Quaternion q) {
		return Quaternion(Cross(m_vec, q.m_vec) + q.w * m_vec + w * q.m_vec, w * q.w - Dot(m_vec, q.m_vec));
	}

	Quaternion Conjugation() {
		return Quaternion(-1.f * m_vec, w);
	}
	float Length() {
		return sqrtf(m_vec.x * m_vec.x + m_vec.y * m_vec.y + m_vec.z * m_vec.z + w * w);
	}
};
#include "Camera.hpp"
#include "../Utility/PathTracerMath.hpp"

#include <iostream>
#include <cmath>
#include <math.h>

PathTracer::Camera::Camera()
	: m_position(0, 0, 0), m_forward(0, 1, 0), m_right(1, 0, 0), m_up(0, 1, 0), m_fovDeg(45)
{
}

PathTracer::Camera::Camera(
	const Vector3& pos, const Vector3& forward, const Vector3& right, const Vector3& up, const float& fovDeg, const unsigned int& width, const unsigned int& height)
	: m_position(pos), m_forward(forward), m_right(right), m_up(up), m_fovDeg(fovDeg)
{
	// アスペクト比
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	float fov = m_fovDeg * (PI / 180.f);
	float imagePlaneHeight = tanf(fov * 0.5f);
	float imagePlaneWidth = aspect * imagePlaneHeight;
	m_right *= imagePlaneWidth;
	m_up *= imagePlaneHeight;
}

PathTracer::Ray PathTracer::Camera::GetCameraRay(float x, float y, unsigned int width, unsigned int height)
{
	// -1 1
	float u = x / static_cast<float>(width) * 2.f - 1.f;
	float v = y / static_cast<float>(height) * 2.f - 1.f;

	// 左上原点にしたいのでvの値を判定させる
	Vector3 rayDir = Normalize(m_right * u + m_up * (-v) + m_forward);

	return Ray(m_position, rayDir);
}

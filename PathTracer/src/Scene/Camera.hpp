#pragma once
#include "../Utility/Vector.hpp"
#include "../Graphics/Ray.hpp"

namespace PathTracer {
	class Camera {
	public:
		Camera();
		Camera(const Vector3& pos, const Vector3& forward, const Vector3& right, const Vector3& up, const float& fovDeg);
		~Camera() = default;

		Ray GetCameraRay(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	private:
		// 位置
		Vector3 m_position;

		// カメラの直行基底ベクトル
		Vector3 m_up;
		Vector3 m_forward;
		Vector3 m_right;

		float m_fovDeg;
	};
}
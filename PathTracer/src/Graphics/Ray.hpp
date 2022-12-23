#pragma once
#include "../Utility/Vector.hpp"

namespace PathTracer {
	class Ray {
	public:
		Ray(const Vector3& origin, const Vector3& direction) : m_origin(origin), m_direction(direction) {};
		~Ray() = default;

		const Vector3& GetOrigin() { return m_origin; }
		const Vector3& GetDirection() { return m_direction; }
	private:
		Vector3 m_origin;
		Vector3 m_direction;
	};
}
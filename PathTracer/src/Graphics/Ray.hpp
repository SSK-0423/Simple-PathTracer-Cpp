#pragma once
#include "../Utility/Vector.hpp"
#include <float.h>

namespace PathTracer {
	class Ray {
	public:
		Ray(const Vector3& origin, const Vector3& direction, const float& maxDistance = FLT_MAX)
			: m_origin(origin), m_direction(direction), m_maxDistance(maxDistance) {};
		~Ray() = default;

		const Vector3& GetOrigin() const { return m_origin; }
		const Vector3& GetDirection() const { return m_direction; }
		const float& GetMaxDistance() const { return m_maxDistance; }
	private:
		Vector3 m_origin;
		Vector3 m_direction;
		float m_maxDistance;
	};
}
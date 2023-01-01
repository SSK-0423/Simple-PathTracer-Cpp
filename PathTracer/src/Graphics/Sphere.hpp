#pragma once
#include "../Utility/Vector.hpp"

namespace PathTracer {
	class Sphere {
	public:
		Sphere() : m_radius(1.f), m_origin(0.f, 0.f, 0.f), m_objectID(0) {}
		Sphere(const float& radius, const Vector3& origin, const unsigned int& objectID)
			: m_radius(radius), m_origin(origin), m_objectID(objectID) {}
		~Sphere() = default;

		float GetRadius() const { return m_radius; }
		Vector3 GetOrigin() const { return m_origin; }
		unsigned int GetObjectID() const { return m_objectID; }
	private:
		float m_radius;
		Vector3 m_origin;
		unsigned int m_objectID;
	};
}
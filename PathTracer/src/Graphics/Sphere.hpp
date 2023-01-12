#pragma once
#include "../Utility/Vector.hpp"
#include "Mesh.hpp"

namespace PathTracer {

	class Sphere : public Mesh {
	public:
		Sphere() : m_radius(1.f), m_origin(0.f, 0.f, 0.f) {
			m_material = Material();
		}
		Sphere(const float& radius, const Vector3& origin, const unsigned int& objectID, const Material& material)
			: m_radius(radius), m_origin(origin) {
			m_objectID = objectID;
			m_material = material;
		}
		~Sphere() = default;

		const IntersectionResult& Intersect(const Ray& ray) override;

		const float& GetRadius() { return m_radius; }
		const Vector3& GetOrigin() { return m_origin; }
		const unsigned int& GetObjectID() { return m_objectID; }
	private:
		float m_radius;
		Vector3 m_origin;
	};
}
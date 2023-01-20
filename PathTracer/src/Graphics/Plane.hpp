#pragma once
#include "../Utility/Vector.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

namespace PathTracer {
	class Plane : public Mesh {
	public:
		Plane(const Vector3& position, const Vector3& normal, const unsigned int& objectID, const Material& material)
			: m_position(position), m_normal(normal)
		{
			m_objectID = objectID;
			m_material = material;
		}
		Plane(const unsigned int& objectID, const Material& material);
		Plane() = default;

		const IntersectionResult Intersect(const Ray& ray) override;
	private:
		Vector3 m_normal;
		Vector3 m_position;
	};
}
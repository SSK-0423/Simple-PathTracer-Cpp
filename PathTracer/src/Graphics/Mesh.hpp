#pragma once
#include "Material.hpp"

namespace PathTracer {
	class IntersectionResult;
	class Ray;

	class Mesh {
	public:
		Mesh();
		~Mesh();

		virtual const IntersectionResult& Intersect(const Ray& ray) = 0;
		const Material& GetMaterial() { return m_material; }
		const unsigned int& GetObjectID() { return m_objectID; }
	protected:
		Material m_material;
		unsigned int m_objectID;
	};
}
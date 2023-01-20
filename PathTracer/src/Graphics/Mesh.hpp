#pragma once
#include <vector>

#include "Material.hpp"
#include "Transform.hpp"
#include "../Utility/Vector.hpp"

namespace PathTracer {
	class IntersectionResult;
	class Ray;

	class Mesh {
	public:
		Mesh();
		~Mesh();

		virtual const IntersectionResult Intersect(const Ray& ray) = 0;
		const Material& GetMaterial() { return m_material; }
		const Transform& GetTransform() { return m_transform; }
		const unsigned int& GetObjectID() { return m_objectID; }
		const std::vector<Vector3>& GetVertices() { return m_vertices; }
		const std::vector<Vector3>& GetNormals() { return m_normals; }
		const std::vector<unsigned int>& GetIndices() { return m_indices; }
	protected:
		Material m_material;
		Transform m_transform;
		unsigned int m_objectID;

		std::vector<Vector3> m_vertices;
		std::vector<unsigned int> m_indices;
		std::vector<Vector3> m_normals;
	};
}
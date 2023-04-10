#pragma once
#include <vector>

#include "Material.hpp"
#include "Transform.hpp"
#include "Polygon.hpp"
#include "../Utility/Vector.hpp"

namespace PathTracer {
	class IntersectionResult;
	class Ray;

	struct Vertex {
		Vector3 position;
		Vector3 normal;
	};

	class Mesh {
	public:
		Mesh();
		~Mesh();

		const Material& GetMaterial() { return m_material; }
		const Transform& GetTransform() { return m_transform; }
		const unsigned int& GetObjectID() { return m_objectID; }
		const TRIANGLE_MASK& GetTriangleMask() { return m_triangleMask; }

		const std::vector<Polygon>& GetPolygons() { return m_polygons; }
	protected:
		Material m_material;
		Transform m_transform;
		unsigned int m_objectID;
		TRIANGLE_MASK m_triangleMask;

		std::vector<Polygon> m_polygons;
		std::vector<Vector3> m_vertices;
		std::vector<unsigned int> m_indices;
		std::vector<Vector3> m_normals;
	};
}
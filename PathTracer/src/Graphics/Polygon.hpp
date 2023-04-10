#pragma once
#include "../Utility/Vector.hpp"
#include <vector>

namespace PathTracer {

	enum class TRIANGLE_MASK {
		GEOMETRY,
		LIGHT,
		NONE,
	};

	class Polygon {
	public:
		Polygon() = default;
		Polygon(
			const Vector3& v0, const Vector3& v1, const Vector3& v2,
			const Vector3& n0, const Vector3& n1, const Vector3& n2,
			const unsigned int& objectID, const TRIANGLE_MASK& mask = TRIANGLE_MASK::GEOMETRY)
			: m_vertices{ v0,v1,v2 }, m_normals{ n0,n1,n2 }, m_objectID(objectID), m_triangleMask(mask)
		{
			m_center = (v0 + v1 + v2) / 3.f;
		}
		~Polygon() = default;

		const unsigned int& GetObjectID() const { return m_objectID; }
		const TRIANGLE_MASK& GetTriangleMask() const { return m_triangleMask; }
		const std::vector<Vector3>& GetVertices() const { return m_vertices; }
		const std::vector<Vector3>& GetNormals() const { return m_normals; }
		const Vector3& GetCenter() const { return m_center; }

	private:
		unsigned int m_objectID;
		TRIANGLE_MASK m_triangleMask;
		Vector3 m_center;
		std::vector<Vector3> m_vertices;
		std::vector<Vector3> m_normals;
	};
}
#include "Plane.hpp"
#include "Ray.hpp"
#include "IntersectionResult.hpp"
#include <float.h>

namespace PathTracer {
	void Plane::CreatePolygons()
	{
		m_vertices = {
			Vector3(-0.5f, 0.5f, 0.f),
			Vector3(0.5f, 0.5f, 0.f),
			Vector3(-0.5f, -0.5f, 0.f),
			Vector3(0.5f, -0.5f, 0.f) };

		m_indices = { 0,1,3,0,3,2 };

		m_normals = { Vector3(0.f,0.f,1.f) };

		m_polygons.resize(m_indices.size() / 3);
		size_t polygonIndex = 0;
		for (size_t i = 0; i < m_indices.size(); i += 3) {
			m_polygons[polygonIndex] = Polygon(
				m_vertices[m_indices[i]], m_vertices[m_indices[i + 1]], m_vertices[m_indices[i + 2]],
				m_normals[0], m_normals[0], m_normals[0],
				m_objectID, m_triangleMask);
			polygonIndex++;
		}
	}
}

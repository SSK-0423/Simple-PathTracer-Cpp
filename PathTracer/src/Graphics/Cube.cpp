#include "Cube.hpp"

namespace PathTracer {
	void Cube::CreatePolygons()
	{
		m_vertices = {
			// positiveX
			Vector3(0.5, -0.5, -0.5),	// 1
			Vector3(0.5, 0.5, -0.5),	// 3
			Vector3(0.5, -0.5, 0.5),	// 5
			Vector3(0.5, 0.5, 0.5),		// 7
			// negativeX
			Vector3(-0.5, -0.5, -0.5),  // 0
			Vector3(-0.5, 0.5, -0.5),   // 2
			Vector3(-0.5, -0.5, 0.5),   // 4
			Vector3(-0.5, 0.5, 0.5),    // 6

			// positiveY
			Vector3(-0.5, 0.5, -0.5),   // 2
			Vector3(0.5, 0.5, -0.5),    // 3
			Vector3(-0.5, 0.5, 0.5),    // 6
			Vector3(0.5, 0.5, 0.5),     // 7
			// negativeY
			Vector3(-0.5, -0.5, -0.5),  // 0
			Vector3(0.5, -0.5, -0.5),   // 1
			Vector3(-0.5, -0.5, 0.5),   // 4
			Vector3(0.5, -0.5, 0.5),    // 5

			// positiveZ
			Vector3(-0.5, -0.5, 0.5),   // 4
			Vector3(0.5, -0.5, 0.5),    // 5
			Vector3(-0.5, 0.5, 0.5),    // 6
			Vector3(0.5, 0.5, 0.5),     // 7
			// negativeZ
			Vector3(-0.5, -0.5, -0.5),  // 0
			Vector3(0.5, -0.5, -0.5),   // 1
			Vector3(-0.5, 0.5, -0.5),   // 2
			Vector3(0.5, 0.5, -0.5),    // 3
		};

		m_normals = {
			// positiveX
			Vector3(1, 0, 0),
			Vector3(1, 0, 0),
			Vector3(1, 0, 0),
			Vector3(1, 0, 0),
			// negativeX
			Vector3(-1, 0, 0),
			Vector3(-1, 0, 0),
			Vector3(-1, 0, 0),
			Vector3(-1, 0, 0),
			// positiveY
			Vector3(0, 1, 0),
			Vector3(0, 1, 0),
			Vector3(0, 1, 0),
			Vector3(0, 1, 0),
			// negativeY
			Vector3(0, -1, 0),
			Vector3(0, -1, 0),
			Vector3(0, -1, 0),
			Vector3(0, -1, 0),
			// positiveZ
			Vector3(0, 0, 1),
			Vector3(0, 0, 1),
			Vector3(0, 0, 1),
			Vector3(0, 0, 1),
			// negativeZ
			Vector3(0, 0, -1),
			Vector3(0, 0, -1),
			Vector3(0, 0, -1),
			Vector3(0, 0, -1)
		};
		m_indices = {
			// positiveX
			0, 1, 3,
			0, 3, 2,
			// negativeX
			4, 5, 7,
			4, 7, 6,
			// positiveY
			8, 10, 11,
			8, 11, 9,
			// negativeY
			12, 14, 15,
			12, 15, 13,
			// positiveZ
			16, 18, 19,
			16, 19, 17,
			// negativeZ
			20, 22, 23,
			20, 23, 21
		};

		m_polygons.resize(m_indices.size() / 3);
		size_t polygonIndex = 0;
		for (size_t i = 0; i < m_indices.size(); i += 3) {
			m_polygons[polygonIndex] = Polygon(
				m_vertices[m_indices[i]], m_vertices[m_indices[i + 1]], m_vertices[m_indices[i + 2]],
				m_normals[m_indices[i]], m_normals[m_indices[i + 1]], m_normals[m_indices[i + 2]],
				m_objectID, m_triangleMask);
			polygonIndex++;
		}
	}
}
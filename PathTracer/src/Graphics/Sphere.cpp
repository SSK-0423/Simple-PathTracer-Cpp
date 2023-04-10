#include "Sphere.hpp"
#include "Ray.hpp"
#include "IntersectionResult.hpp"
#include "../Utility/PathTracerMath.hpp"
#include <float.h>

namespace PathTracer {
	void Sphere::CreateVertexAttributes(const float& radius, const unsigned int& stackCount, const unsigned int& sectorCount)
	{
		const float sectorStep = 2 * PI / static_cast<float>(sectorCount);
		const float stackStep = PI / static_cast<float>(stackCount);

		float sectorAngle = 0.f;
		float stackAngle = 0.f;
		float lengthInv = 1.f / radius;

		for (size_t stack = 0; stack < (stackCount + 1); stack++) {
			stackAngle = PI / 2.f - stackStep * static_cast<float>(stack);
			float xz = radius * cosf(stackAngle);
			float y = radius * sinf(stackAngle);

			for (size_t sector = 0; sector < (sectorCount + 1); sector++) {
				sectorAngle = sectorStep * static_cast<float>(sector);
				// 頂点
				float z = xz * sinf(sectorAngle);
				float x = xz * cosf(sectorAngle);
				// 頂点
				m_vertices.push_back(Vector3(x, y, z));
				// 法線
				m_normals.push_back(Vector3(x * lengthInv, y * lengthInv, z * lengthInv));
				// UVが必要になれば使う
				//float u = static_cast<float>(sector) / static_cast<float>(sectorCount);
				//float v = 1.f - static_cast<float>(stack) / static_cast<float>(stackCount);  //UVは左下(0,0)
			}
		}
	}
	void Sphere::CreateIndices(const unsigned int& stackCount, const unsigned int& sectorCount)
	{
		unsigned int k1 = 0;
		unsigned int k2 = 0;

		for (size_t stack = 0; stack < stackCount; stack++) {
			k1 = stack * (sectorCount + 1);
			k2 = k1 + sectorCount + 1;
			for (size_t sector = 0; sector < sectorCount; sector++) {
				// 1セクション毎に2つの三角形を生成
				// k1 => k2 => k1+1
				if (stack != 0) {
					m_indices.push_back(k1);
					m_indices.push_back(k2);
					m_indices.push_back(k1 + 1);
				}
				// k1+1 => k2 => k2 + 1
				if (stack != (stackCount - 1)) {
					m_indices.push_back(k1 + 1);
					m_indices.push_back(k2);
					m_indices.push_back(k2 + 1);
				}
				k1 += 1;
				k2 += 1;
			}
		}
	}
	void Sphere::CreatePolygons()
	{
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
#pragma once
#include "../Utility/Vector.hpp"
#include "Mesh.hpp"
#include "IntersectionResult.hpp"
namespace PathTracer {

	class Triangle : public Mesh {
	public:
		Triangle() {
			m_material = Material();
			m_vertices.push_back(Vector3(0.f, 0.5f, 0.f));
			m_vertices.push_back(Vector3(-0.5f, -0.5f, 0.f));
			m_vertices.push_back(Vector3(0.5f, -0.5f, 0.f));

			m_indices.push_back(0);
			m_indices.push_back(1);
			m_indices.push_back(2);
		}
		Triangle(const unsigned int& objectID, const Material& material, const Transform& transform) {
			m_objectID = objectID;
			m_material = material;
			m_transform = transform;

			m_vertices.push_back(Vector3(0.f, 0.5f, 0.f));
			m_vertices.push_back(Vector3(-0.5f, -0.5f, 0.f));
			m_vertices.push_back(Vector3(0.5f, -0.5f, 0.f));

			m_indices.push_back(0);
			m_indices.push_back(1);
			m_indices.push_back(2);
		}
		~Triangle() = default;
	};
}
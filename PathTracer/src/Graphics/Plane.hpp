#pragma once
#include "../Utility/Vector.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

namespace PathTracer {
	class Plane : public Mesh {
	public:
		Plane(const Transform& transform, const Material& material, const unsigned int& objectID)
		{
			m_transform = transform;
			m_material = material;
			m_objectID = objectID;

			m_vertices = {
				Vector3(-0.5f, 0.5f, 0.f),
				Vector3(0.5f, 0.5f, 0.f),
				Vector3(-0.5f, -0.5f, 0.f),
				Vector3(0.5f, -0.5f, 0.f) };

			m_indices = { 0,1,3,0,3,2 };

			m_normals = {
				Vector3(0.f,0.f,1.f),
				Vector3(0.f,0.f,1.f),
				Vector3(0.f,0.f,1.f),
				Vector3(0.f,0.f,1.f) };
		}
		Plane() = default;
	};
}
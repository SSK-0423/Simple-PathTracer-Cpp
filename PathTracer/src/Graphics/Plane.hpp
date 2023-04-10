#pragma once
#include "../Utility/Vector.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

namespace PathTracer {
	class Plane : public Mesh {
	public:
		Plane(const Transform& transform, const Material& material, const unsigned int& objectID, const TRIANGLE_MASK& mask = TRIANGLE_MASK::GEOMETRY)
		{
			m_transform = transform;
			m_material = material;
			m_objectID = objectID;
			m_triangleMask = mask;

			CreatePolygons();
		}
		Plane() = default;
	private:
		void CreatePolygons();
	};
}
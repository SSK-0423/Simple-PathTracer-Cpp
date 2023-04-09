#pragma once
#include "Mesh.hpp"

namespace PathTracer {
	class Cube : public Mesh {
	public:
		Cube(const Transform& transform, const Material& material, const unsigned int& objectID, const TRIANGLE_MASK& mask = TRIANGLE_MASK::GEOMETRY)
		{
			m_objectID = objectID;
			m_material = material;
			m_transform = transform;
			m_triangleMask = mask;

			CreatePolygons();
		}

		~Cube() = default;

	private:
		void CreatePolygons();
	};
}
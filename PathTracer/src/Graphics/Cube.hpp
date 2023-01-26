#pragma once
#include "Mesh.hpp"

namespace PathTracer {
	class Cube : public Mesh {
	public:
		Cube(const Transform& transform, const Material& material, const unsigned int& objectID)
		{
			m_objectID = objectID;
			m_material = material;
			m_transform = transform;

			CreateVertexAttributes();
			CreateIndices();
		}

		~Cube() = default;

	private:
		void CreateVertexAttributes();
		void CreateIndices();
	};
}
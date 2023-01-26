#pragma once
#include "../Utility/Vector.hpp"
#include "Mesh.hpp"

namespace PathTracer {

	class Sphere : public Mesh {
	public:
		Sphere(
			const float& radius, const unsigned int& stackCount, const unsigned int& sectorCount,
			const Transform& transform, const Material& material, const unsigned int& objectID)
		{
			m_objectID = objectID;
			m_material = material;
			m_transform = transform;

			CreateVertexAttributes(radius, stackCount, sectorCount);
			CreateIndices(stackCount, sectorCount);
		}
		~Sphere() = default;

	private:
		void CreateVertexAttributes(const float& radius, const unsigned int& stackCount, const unsigned int& sectorCount);
		void CreateIndices(const unsigned int& stackCount, const unsigned int& sectorCount);
	};
}
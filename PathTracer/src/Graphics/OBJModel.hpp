#pragma once
#include "Mesh.hpp"
#include <string>

namespace PathTracer {
	class OBJModel : public Mesh {
	public:
		OBJModel(
			const std::string& fileName, const Transform& transform, const Material& material,
			const unsigned int& objectID, const TRIANGLE_MASK& mask = TRIANGLE_MASK::GEOMETRY)
		{
			m_objectID = objectID;
			m_material = material;
			m_transform = transform;
			m_triangleMask = mask;

			LoadObjFile(fileName);
		}
		~OBJModel() = default;

	private:
		void LoadObjFile(const std::string& fileName);
	};
}
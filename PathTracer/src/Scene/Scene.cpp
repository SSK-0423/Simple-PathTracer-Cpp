#include "Scene.hpp"
#include "../Graphics/Mesh.hpp"
#include "../Graphics/Sphere.hpp"

namespace PathTracer {

	std::shared_ptr<Mesh> Scene::GetMesh(unsigned int objectID) const
	{
		for (auto mesh : m_meshes) {
			if (mesh->GetObjectID() == objectID) {
				return mesh;
			}
		}

		return nullptr;
	}

	std::shared_ptr<Sphere> Scene::GetSphere(unsigned int objectID) const
	{
		for (auto sphere : m_spheres) {
			if (sphere->GetObjectID() == objectID)
				return sphere;
		}

		return nullptr;
	}
}

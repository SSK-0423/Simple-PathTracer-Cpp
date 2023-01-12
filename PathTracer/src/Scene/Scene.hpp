#pragma once
#include <memory>
#include <vector>

namespace PathTracer {
	class Mesh;
	class Sphere;

	class Scene {
	public:
		Scene() = default;
		virtual ~Scene() = default;

		virtual void Init() = 0;
		std::shared_ptr<Mesh> GetMesh(unsigned int meshID) const;
		std::shared_ptr<Sphere> GetSphere(unsigned int objectID) const;

		const std::vector<std::shared_ptr<Sphere>>& GetSpheres() const { return m_spheres; }
		const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_meshes; }
	protected:
		std::vector<std::shared_ptr<Mesh>> m_meshes;
		std::vector<std::shared_ptr<Sphere>> m_spheres;
	};
}
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

		const std::vector<std::shared_ptr<Sphere>>& GetSpheres() const { return m_spheres; }
	protected:
		std::vector<std::shared_ptr<Mesh>> m_meshes;
		std::vector<std::shared_ptr<Sphere>> m_spheres;
	};
}
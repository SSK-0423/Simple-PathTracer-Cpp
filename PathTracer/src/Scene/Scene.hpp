#pragma once
#include <memory>
#include <vector>

namespace PathTracer {
	class Mesh;
	class LightSource;

	class Scene {
	public:
		Scene() = default;
		virtual ~Scene() = default;

		virtual void Init() = 0;
		std::shared_ptr<Mesh> GetMesh(unsigned int meshID) const;

		const std::vector<std::shared_ptr<LightSource>>& GetLightSources() const { return m_lights; }
		const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_meshes; }
	protected:
		std::vector<std::shared_ptr<Mesh>> m_meshes;
		std::vector<std::shared_ptr<LightSource>> m_lights;
	};
}
#pragma once
#include <memory>
#include <vector>

#include "BVH.hpp"

namespace PathTracer {
	class Mesh;
	class LightSource;

	class Scene {
	public:
		Scene() = default;
		virtual ~Scene() = default;

		virtual void Init() = 0;
		std::shared_ptr<Mesh> GetMesh(unsigned int meshID) const;

		const BVH& GetBVH() const { return m_bvh; }
		const std::vector<std::shared_ptr<LightSource>>& GetLightSources() const { return m_lights; }
		const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_meshes; }
	protected:
		BVH m_bvh;
		std::vector<std::shared_ptr<Mesh>> m_meshes;
		std::vector<std::shared_ptr<LightSource>> m_lights;
	};
}
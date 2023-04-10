#pragma once
#include <vector>

#include "../Utility/Vector.hpp"
#include "../Graphics/Polygon.hpp"
#include "../Graphics/Cube.hpp"

namespace PathTracer {
	class Ray;
	class Scene;

	class AABB {
	public:
		AABB()
			:m_center(0.f, 0.f, 0.f), m_maxCoord(-FLT_MAX, -FLT_MAX, -FLT_MAX), m_minCoord(FLT_MAX, FLT_MAX, FLT_MAX),
			m_childIndex(), m_children(), m_polygons()
		{};
		AABB(std::vector<Polygon>& polygons);
		AABB(const Polygon& polygon);
		~AABB() = default;

		bool Intersect(const Ray& ray) const;
		bool IsLeafNode() const {
			if (m_childIndex[0] == -1 && m_childIndex[1] == -1)
				return true;
			return false;
		}
		void SetChildren(const AABB& aabb) { m_children.push_back(aabb); };
		void SetChildIndex(int child0, int child1) { m_childIndex[0] = child0; m_childIndex[1] = child1; }
		void SetPolygons(const std::vector<Polygon>& polygons) { m_polygons = polygons; };
		const Vector3& GetCenter() const { return m_center; }
		const Vector3& GetMaxCoord() const { return m_maxCoord; }
		const Vector3& GetMinCoord() const { return m_minCoord; }
		const std::vector<AABB>& GetChildren() const { return m_children; }
		const std::vector<Polygon>& GetPolygons() const { return m_polygons; }
		const int* const GetChildIndex() const { return m_childIndex; }
		const std::vector<Polygon>& GetAABBPolygons() const { return m_aabbPolygons; }
		float SurfaceArea() const;
		static AABB Merge(const AABB& lhs, const AABB& rhs);
	private:
		Vector3 m_center;
		Vector3 m_maxCoord;
		Vector3 m_minCoord;

		int m_childIndex[2];
		std::vector<AABB> m_children;
		std::vector<Polygon> m_polygons;

		std::vector<Polygon> m_aabbPolygons;
		void CreateAABBPolygon();
	};

	class BVH {
	public:
		BVH() = default;
		~BVH() = default;

		void Construct(const Scene& scene);

		const std::vector<AABB>& GetNodes() const { return m_nodes; }
		const AABB& GetRootNode() const { return m_nodes[0]; }

	private:
		AABB m_rootNode;
		std::vector<AABB> m_nodes;

		int m_usedNodeIndex = 0;

		void CreateTransformedPolygons(const Scene& scene, std::vector<Polygon>& polygons);
		void ConstructBVH(std::vector<Polygon>& polygons, int nodeIndex, int depth);
	};
}
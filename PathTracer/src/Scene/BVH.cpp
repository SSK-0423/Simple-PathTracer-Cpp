#include "BVH.hpp"
#include "Scene.hpp"
#include "../Graphics/Mesh.hpp"
#include "../Graphics/Ray.hpp"
#include <algorithm>
namespace PathTracer {
	constexpr float T_tri = 1.f;
	constexpr float T_aabb = 1.f;
	constexpr int MAX_DEPTH = 4;

	AABB::AABB(std::vector<Polygon>& polygons)
		: m_polygons(polygons), m_center(0.f, 0.f, 0.f), m_maxCoord(-FLT_MAX, -FLT_MAX, -FLT_MAX), m_minCoord(FLT_MAX, FLT_MAX, FLT_MAX)
	{
		AABB mergedAABB = AABB();
		// ポリゴンのAABBを生成してマージ
		for (auto polygon : polygons) {
			mergedAABB = AABB::Merge(mergedAABB, AABB(polygon));
		}
		m_maxCoord = mergedAABB.m_maxCoord;
		m_minCoord = mergedAABB.m_minCoord;
		m_center = mergedAABB.m_center;

		for (size_t axis = 0; axis < 3; axis++) {
			// 平面など最大値と最小値が同じになる場合は
			// 微小成分だけ厚みを持たせる
			if (m_minCoord[axis] == m_maxCoord[axis]) {
				m_minCoord[axis] -= EPSILON;
				m_maxCoord[axis] += EPSILON;
			}
		}
	}
	AABB::AABB(const Polygon& polygon)
		: m_center(0.f, 0.f, 0.f), m_maxCoord(-FLT_MAX, -FLT_MAX, -FLT_MAX), m_minCoord(FLT_MAX, FLT_MAX, FLT_MAX)
	{
		// 軸毎に最大最小値を見つける
		const std::vector<Vector3> vertices = polygon.GetVertices();
		for (auto vertex : vertices) {
			for (size_t axis = 0; axis < 3; axis++) {
				m_minCoord[axis] = std::min(m_minCoord[axis], vertex[axis]);
				m_maxCoord[axis] = std::max(m_maxCoord[axis], vertex[axis]);
			}
		}

		for (size_t axis = 0; axis < 3; axis++) {
			// 平面など最大値と最小値が同じになる場合は
			// 微小成分だけ厚みを持たせる
			if (m_minCoord[axis] == m_maxCoord[axis]) {
				m_minCoord[axis] -= EPSILON;
				m_maxCoord[axis] += EPSILON;
			}
		}

		// 中心座標をセット
		m_center = Vector3(
			(m_maxCoord[0] + m_minCoord[0]) / 2.f,
			(m_maxCoord[1] + m_minCoord[1]) / 2.f,
			(m_maxCoord[2] + m_minCoord[2]) / 2.f);
	}
	bool AABB::Intersect(const Ray& ray) const
	{
		// TODO: 実装する
		auto& rayOrigin = ray.GetOrigin();
		auto& rayDir = ray.GetDirection();

		float t_max = FLT_MAX;   // AABB からレイが外に出る時刻
		float t_min = -FLT_MAX;  // AABB にレイが侵入する時刻

		for (int axis = 0; axis < 3; axis++) {
			float divDir = 1.f / ray.GetDirection()[axis];
			float t1 = (m_minCoord[axis] - rayOrigin[axis]) * divDir;
			float t2 = (m_maxCoord[axis] - rayOrigin[axis]) * divDir;

			if (divDir < 0.f)
				std::swap(t1, t2);

			float t_near = std::min(t1, t2);
			float t_far = std::max(t1, t2);

			t_max = std::min(t_max, t_far);
			t_min = std::max(t_min, t_near);

			// レイが外に出る時刻と侵入する時刻が逆転している => 交差していない
			if (t_min > t_max) return false;
		}
		return true;

		//// 各軸毎の到達間隔を計算
		//float min = -FLT_MAX;
		//float max = FLT_MAX;

		//for (size_t axis = 0; axis < 3; axis++) {
		//	// ゼロ除算対策
		//	if (std::abs(rayDir[axis]) < EPSILON) {
		//		if (rayOrigin[axis] < m_minCoord[axis] || rayOrigin[axis] > m_maxCoord[axis]) {
		//			return false;
		//		}
		//	}
		//	float invDir = 1.f / rayDir[axis];
		//	float near = (m_minCoord[axis] - rayOrigin[axis]) * invDir;
		//	float far = (m_maxCoord[axis] - rayOrigin[axis]) * invDir;
		//	// 値が大きい方からレイが来た時はnearとfarが反転する
		//	if (invDir < 0.f) {
		//		std::swap(near, far);
		//	}
		//	min = near > min ? near : min;
		//	max = far < max ? far : max;

		//	// 各軸毎に交差をチェックする
		//	if (max <= min)
		//		return false;
		//}
		//return true;
	}
	float AABB::SurfaceArea() const
	{
		float dx = std::fabs(m_maxCoord[0] - m_minCoord[0]);
		float dy = std::fabs(m_maxCoord[1] - m_minCoord[1]);
		float dz = std::fabs(m_maxCoord[2] - m_minCoord[2]);

		return 2.f * (dx * dy + dx * dz + dy * dz);
	}

	AABB AABB::Merge(const AABB& lhs, const AABB& rhs)
	{
		AABB ret;
		for (size_t axis = 0; axis < 3; axis++) {
			ret.m_minCoord[axis] = std::min(lhs.m_minCoord[axis], rhs.m_minCoord[axis]);
			ret.m_maxCoord[axis] = std::max(lhs.m_maxCoord[axis], rhs.m_maxCoord[axis]);
		}
		ret.m_center = Vector3(
			(ret.m_maxCoord[0] + ret.m_minCoord[0]) / 2.f,
			(ret.m_maxCoord[1] + ret.m_minCoord[1]) / 2.f,
			(ret.m_maxCoord[2] + ret.m_minCoord[2]) / 2.f);
		ret.CreateAABBPolygon();
		return ret;
	}
	void AABB::CreateAABBPolygon()
	{
		float maxX = m_maxCoord[0];
		float maxY = m_maxCoord[1];
		float maxZ = m_maxCoord[2];

		float minX = m_minCoord[0];
		float minY = m_minCoord[1];
		float minZ = m_minCoord[2];

		std::vector<Vector3> m_vertices = {
			// positiveX
			Vector3(maxX, minY, minZ),	    // 1
			Vector3(maxX, maxY, minZ),	    // 3
			Vector3(maxX, minY, maxZ),	    // 5
			Vector3(maxX, maxY, maxZ),		// 7
			// negativeX
			Vector3(minX, minY, minZ),      // 0
			Vector3(minX, maxY, minZ),      // 2
			Vector3(minX, minY, maxZ),      // 4
			Vector3(minX, maxY, maxZ),      // 6

			// positiveY
			Vector3(minX, maxY, minZ),      // 2
			Vector3(maxX, maxY, minZ),      // 3
			Vector3(minX, maxY, maxZ),      // 6
			Vector3(maxX, maxY, maxZ),      // 7
			// negativeY
			Vector3(minX, minY, minZ),      // 0
			Vector3(maxX, minY, minZ),      // 1
			Vector3(minX, minY, maxZ),      // 4
			Vector3(maxX, minY, maxZ),      // 5

			// positiveZ
			Vector3(minX, minY, maxZ),      // 4
			Vector3(maxX, minY, maxZ),      // 5
			Vector3(minX, maxY, maxZ),      // 6
			Vector3(maxX, maxY, maxZ),      // 7
			// negativeZ
			Vector3(minX, minY, minZ),      // 0
			Vector3(maxX, minY, minZ),      // 1
			Vector3(minX, maxY, minZ),      // 2
			Vector3(maxX, maxY, minZ),      // 3
		};

		std::vector<Vector3> m_normals = {
			// positiveX
			Vector3(1, 0, 0),
			Vector3(1, 0, 0),
			Vector3(1, 0, 0),
			Vector3(1, 0, 0),
			// negativeX
			Vector3(-1, 0, 0),
			Vector3(-1, 0, 0),
			Vector3(-1, 0, 0),
			Vector3(-1, 0, 0),
			// positiveY
			Vector3(0, 1, 0),
			Vector3(0, 1, 0),
			Vector3(0, 1, 0),
			Vector3(0, 1, 0),
			// negativeY
			Vector3(0, -1, 0),
			Vector3(0, -1, 0),
			Vector3(0, -1, 0),
			Vector3(0, -1, 0),
			// positiveZ
			Vector3(0, 0, 1),
			Vector3(0, 0, 1),
			Vector3(0, 0, 1),
			Vector3(0, 0, 1),
			// negativeZ
			Vector3(0, 0, -1),
			Vector3(0, 0, -1),
			Vector3(0, 0, -1),
			Vector3(0, 0, -1)
		};
		std::vector<unsigned int> m_indices = {
			// positiveX
			0, 1, 3,
			0, 3, 2,
			// negativeX
			4, 5, 7,
			4, 7, 6,
			// positiveY
			8, 10, 11,
			8, 11, 9,
			// negativeY
			12, 14, 15,
			12, 15, 13,
			// positiveZ
			16, 18, 19,
			16, 19, 17,
			// negativeZ
			20, 22, 23,
			20, 23, 21
		};

		m_aabbPolygons.resize(m_indices.size() / 3);
		size_t polygonIndex = 0;
		for (size_t i = 0; i < m_indices.size(); i += 3) {
			m_aabbPolygons[polygonIndex] = Polygon(
				m_vertices[m_indices[i]], m_vertices[m_indices[i + 1]], m_vertices[m_indices[i + 2]],
				m_normals[m_indices[i]], m_normals[m_indices[i + 1]], m_normals[m_indices[i + 2]],
				-1, TRIANGLE_MASK::NONE);
			polygonIndex++;
		}
	}

	void BVH::Construct(const Scene& scene)
	{
		std::vector<Polygon> polygons = std::vector<Polygon>();
		// 座標変換を適用したポリゴンのリストを作成
		CreateTransformedPolygons(scene, polygons);

		printf("総ポリゴン数: %d\n", polygons.size());

		m_nodes.resize(powf(2, MAX_DEPTH + 1) - 1);

		// BVH構築
		ConstructBVH(polygons, 0, 0);

		printf("\nBVH構築完了\n");

		printf("BVHツリー表示\n");

		int index = 0;
		for (auto node : m_nodes) {
			printf("Node%2d : %4d Polygons IsLeaf : ", index, node.GetPolygons().size());
			if (node.IsLeafNode())
				printf("true\n");
			else
				printf("false\n");
			index++;
		}

		printf("--------------------------------\n");
	}
	void BVH::CreateTransformedPolygons(const Scene& scene, std::vector<Polygon>& polygons)
	{
		size_t polygonCount = 0;
		for (auto mesh : scene.GetMeshes()) {
			polygonCount += mesh->GetPolygons().size();
		}
		polygons.resize(polygonCount);

		size_t index = 0;
		for (auto mesh : scene.GetMeshes()) {
			Matrix4x4 modelMatrix = mesh->GetTransform().GetModelMatrix();

			for (auto polygon : mesh->GetPolygons()) {
				const std::vector<Vector3>& polygonVertices = polygon.GetVertices();
				const std::vector<Vector3>& polygonNormals = polygon.GetNormals();
				std::vector<Vector3> transformedVertices(polygonVertices.size());
				std::vector<Vector3> transformedNormals(polygonNormals.size());

				for (size_t i = 0; i < 3; i++) {
					Vector4 pos = Vector4(polygonVertices[i], 1.f);
					transformedVertices[i] = (modelMatrix * pos).xyz();
					transformedNormals[i] = Normalize(modelMatrix * Vector4(polygonNormals[i], 0.f)).xyz();
				}

				polygons[index] = Polygon(
					transformedVertices[0], transformedVertices[1], transformedVertices[2],
					transformedNormals[0], transformedNormals[1], transformedNormals[2],
					mesh->GetObjectID(), mesh->GetTriangleMask());

				index++;
			}
		}
	}
	void BVH::ConstructBVH(std::vector<Polygon>& polygons, int nodeIndex, int depth)
	{
		// 全体を囲うAABBを構築
		AABB parent = AABB(polygons);
		m_nodes[nodeIndex] = parent;

		// 木の深さが最大値に達したら
		if (depth == MAX_DEPTH) {
			// 葉ノード作成
			m_nodes[nodeIndex].SetChildIndex(-1, -1);
			m_nodes[nodeIndex].SetPolygons(polygons);
			return;
		}

		// 現在のAABBを葉ノードとしたときのコストで初期化
		float bestCost = T_tri * polygons.size();

		int bestAxis = -1;	// 分割に最もよい軸(0:x 1:y 2:z)
		int bestSplitIndex = -1; // 分割に最もよい場所
		float parentSA = parent.SurfaceArea();

		// SAHでポリゴンを分割
		// 軸毎にSAHを評価して分割に最適な軸を決める
		for (size_t axis = 0; axis < 3; axis++) {
			// ポリゴンを１つの軸に注目して昇順ソート
			std::sort(polygons.begin(), polygons.end(),
				[axis](const Polygon& lhs, const Polygon& rhs) {
					return lhs.GetCenter()[axis] < rhs.GetCenter()[axis];
					//return AABB(lhs).GetCenter()[axis] < AABB(rhs).GetCenter()[axis];
				});
			//
			std::vector<Polygon> s1;
			std::vector<Polygon> s2(polygons);

			// AABBの表面積リスト
			// s1SA[i], s2SA[i]は、「S1側にi個，S2側に(polygons.size() - i)個ポリゴンがあるように分割」したときの表面積
			std::vector<float> s1SA(polygons.size() + 1, FLT_MAX);
			std::vector<float> s2SA(polygons.size() + 1, FLT_MAX);

			AABB s1AABB = AABB();
			// S1側にポリゴンが0個～全部の場合のS1のAABBの表面積を計算
			for (int i = 0; i <= polygons.size(); i++) {
				s1SA[i] = fabs(s1AABB.SurfaceArea());	// 現在のS1のAABBの表面積を計算
				if (s2.size() > 0) {
					// s2側で、注目する軸の最左(最小位置)にいるポリゴンをS1の最右(最大位置)に移す
					Polygon polygon = s2.front();
					s1.push_back(polygon);
					// 先頭削除
					s2.erase(s2.begin());
					// AABB更新
					s1AABB = AABB::Merge(s1AABB, AABB(polygon));
				}
			}

			AABB s2AABB = AABB();
			// S2側にポリゴンが0個～全部の場合のS2のAABBの表面積を計算
			// すると同時にSAHを計算
			for (int i = polygons.size(); i >= 0; i--) {
				s2SA[i] = fabs(s2AABB.SurfaceArea());	// 現在のS2のAABBの表面積を計算
				if (s1.size() > 0 && s2.size() > 0) {
					// SAHのコスト計算
					// 2×AABBの交差判定にかかる時間 + 
					// (領域1のAABBの表面積×領域1のポリゴンの数 + 領域2のAABBの表面積×領域2のポリゴンの数) × 
					// ポリゴンの交差判定にかかる時間 / 領域全体のAABBの表面積
					float cost =
						2 * T_aabb + (s1SA[i] * s1.size() + s2SA[i] * s2.size()) * T_tri / parentSA;

					// 最小コストの更新
					if (cost < bestCost) {
						bestCost = cost;
						bestAxis = axis;
						bestSplitIndex = i;
					}
				}

				if (s1.size() > 0) {
					// s1側で、注目する軸の最右(最大位置)にいるポリゴンをS2の最左(最小位置)に移す
					Polygon polygon = s1.back();
					s2.insert(s2.begin(), polygon);
					// 末尾削除
					s1.pop_back();
					// AABB更新
					s2AABB = AABB::Merge(s2AABB, AABB(polygon));
				}
			}
			printf("//");
		}

		// 現在のノードを葉ノードとするのが最良と判断された
		if (bestAxis == -1) {
			// 葉ノード作成
			m_nodes[nodeIndex].SetChildIndex(-1, -1);
			m_nodes[nodeIndex].SetPolygons(polygons);
		}
		else {
			// 最良と判断された軸でポリゴンを昇順ソート
			std::sort(polygons.begin(), polygons.end(),
				[bestAxis](const Polygon& lhs, const Polygon& rhs) {
					return lhs.GetCenter()[bestAxis] < rhs.GetCenter()[bestAxis];
					//return AABB(lhs).GetCenter()[bestAxis] < AABB(rhs).GetCenter()[bestAxis];
				});
			std::vector<Polygon> right(polygons.begin(), polygons.begin() + bestSplitIndex);
			std::vector<Polygon> left(polygons.begin() + bestSplitIndex, polygons.end());

			m_nodes[nodeIndex].SetChildIndex(m_usedNodeIndex + 1, m_usedNodeIndex + 2);
			m_usedNodeIndex += 2;

			//再帰的の子ノードを作成
			ConstructBVH(right, m_nodes[nodeIndex].GetChildIndex()[0], depth + 1);
			ConstructBVH(left, m_nodes[nodeIndex].GetChildIndex()[1], depth + 1);
		}
	}
}
#include "Intersector.hpp"
#include "IntersectionResult.hpp"
#include "Ray.hpp"
#include "Mesh.hpp"
#include "../Scene/Scene.hpp"

namespace PathTracer {
	constexpr float EP = 0.1f;//0.025f;

	Intersector::Intersector()
	{
	}
	Intersector::~Intersector()
	{
	}

	IntersectionResult Intersector::Intersect(const Ray& ray, const Scene& scene, bool isWireFrame, bool exitOnceFound)
	{
		float minDistance = FLT_MAX;
		IntersectionResult intersectResult = IntersectionResult();

		for (auto mesh : scene.GetMeshes()) {
			Matrix4x4 modelMatrix = mesh->GetTransform().GetModelMatrix();

			for (auto& polygon : mesh->GetPolygons()) {
				const std::vector<Vector3>& polygonVertices = polygon.GetVertices();
				std::vector<Vector3> transformedVertices(polygonVertices.size());

				// 座標変換
				for (size_t i = 0; i < 3; i++) {
					Vector4 pos = Vector4(polygonVertices[i], 1.f);
					transformedVertices[i] = (modelMatrix * pos).xyz();
				}

				float t = 0.f;
				// バリセントリック座標(u,v)
				float u = 0.f;
				float v = 0.f;
				INTERSECTION_TYPE type = IntersectTriangle(ray, transformedVertices[0], transformedVertices[1], transformedVertices[2], t, u, v);

				if (type == INTERSECTION_TYPE::NONE) continue;

				// 三角形のエッジではない部分の交差判定は無視する
				if (isWireFrame) {
					if (u > EPSILON && v > EPSILON && (1.f - u - v) > EPSILON) {
						continue;
					}
				}

				float distance = (t * ray.GetDirection()).Length();
				if (distance > ray.GetMaxDistance()) continue;

				// シャドウイング用
				if (exitOnceFound) {
					return IntersectionResult(Vector3(), Vector3(), FLT_MAX, -1, TRIANGLE_MASK::NONE, type);
				}

				if (distance < minDistance) {
					Vector3 pos = ray.GetOrigin() + t * ray.GetDirection();

					// バリセントリック座標を用いて交差点における法線を算出する
					const std::vector<Vector3>& polygonNormals = polygon.GetNormals();
					Vector3 normal = Normalize(polygonNormals[1] * u + polygonNormals[2] * v + polygonNormals[0] * (1.f - u - v));
					normal = Normalize(modelMatrix * Vector4(normal, 0.f)).xyz();

					minDistance = distance;
					intersectResult = IntersectionResult(pos, normal, distance, mesh->GetObjectID(), mesh->GetTriangleMask(), type);
				}
			}
		}
		return intersectResult;
	}

	IntersectionResult Intersector::IntersectBVH(const Ray& ray, const Scene& scene, bool exitOnceFound)
	{
		BVH bvh = scene.GetBVH();
		const std::vector<AABB> bvhNodes = bvh.GetNodes();
		AABB node = bvhNodes[0];

		IntersectionResult result = IntersectionResult();

		while (true) {
			// 親ノード交差判定
			if (node.Intersect(ray)) {
				// 葉ノードかどうか
				if (node.IsLeafNode()) {
					// 交差するポリゴンを判定する
					return IntersctPolygons(ray, node.GetPolygons(), exitOnceFound);
				}
				const int* const childIndex = node.GetChildIndex();

				// 子ノード交差判定
				if (bvhNodes[childIndex[0]].Intersect(ray)) {
					node = bvhNodes[childIndex[0]];
				}
				else if (bvhNodes[childIndex[1]].Intersect(ray)) {
					node = bvhNodes[childIndex[1]];
				}
				continue;
			}
			break;
		}

		return result;
	}

	IntersectionResult Intersector::IntersectBVH(const Ray& ray, const std::vector<AABB>& bvhNodes, int nodeIndex, bool isWireFrame, bool exitOnceFound)
	{
		const AABB& node = bvhNodes[nodeIndex];
		bool isIntersect = node.Intersect(ray);

		// 親ノード交差判定
		if (isIntersect) {
			// 葉ノードかどうか
			if (node.IsLeafNode()) {
				// 交差するポリゴンを判定する
				return IntersctPolygons(ray, node.GetPolygons(), isWireFrame, exitOnceFound);
			}
			const int* const childIndex = node.GetChildIndex();

			// 子ノード交差判定
			IntersectionResult result1 = IntersectBVH(ray, bvhNodes, childIndex[0], exitOnceFound);
			IntersectionResult result2 = IntersectBVH(ray, bvhNodes, childIndex[1], exitOnceFound);

			// 子ノードの交差判定結果で一番手前のモノを採用
			if (result1.GetDistance() < result2.GetDistance())
				return result1;

			return result2;
		}
		return IntersectionResult();
	}

	IntersectionResult Intersector::IntersctPolygons(const Ray& ray, const std::vector<Polygon>& polygons, bool isWireFrame, bool exitOnceFound)
	{
		float minDistance = FLT_MAX;
		IntersectionResult intersectResult = IntersectionResult();

		// 交差するポリゴンを判定する
		for (auto& polygon : polygons) {
			const std::vector<Vector3>& vertices = polygon.GetVertices();

			float t = 0.f;
			// バリセントリック座標(u,v)
			float u = 0.f;
			float v = 0.f;
			INTERSECTION_TYPE type = IntersectTriangle(ray, vertices[0], vertices[1], vertices[2], t, u, v);

			if (type == INTERSECTION_TYPE::NONE) continue;

			// 三角形のエッジではない部分の交差判定は無視する
			if (isWireFrame) {
				if (u > EPSILON && v > EPSILON && (1.f - u - v) > EPSILON) {
					continue;
				}
			}

			float distance = (t * ray.GetDirection()).Length();
			if (distance > ray.GetMaxDistance()) continue;

			// シャドウイング用
			if (exitOnceFound && polygon.GetTriangleMask() != TRIANGLE_MASK::GEOMETRY) {
				return IntersectionResult(Vector3(), Vector3(), FLT_MAX, -1, TRIANGLE_MASK::NONE, type);
			}

			if (distance <= minDistance) {
				Vector3 pos = ray.GetOrigin() + t * ray.GetDirection();

				// バリセントリック座標を用いて交差点における法線を算出する
				const std::vector<Vector3>& polygonNormals = polygon.GetNormals();
				Vector3 normal = Normalize(polygonNormals[1] * u + polygonNormals[2] * v + polygonNormals[0] * (1.f - u - v));
				minDistance = distance;
				intersectResult = IntersectionResult(pos, normal, distance, polygon.GetObjectID(), polygon.GetTriangleMask(), type);
			}
		}

		return intersectResult;
	}

	INTERSECTION_TYPE Intersector::IntersectTriangle(const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2, float& enlarge, float& barycentricU, float& barycentricV)
	{
		// [Moller97]の手法
		// 表裏どちらからでも交差する実装になっている

		Vector3 dir = ray.GetDirection();
		Vector3 edge1 = v1 - v0;
		Vector3 edge2 = v2 - v0;
		Vector3 originDir = ray.GetOrigin() - v0;

		Vector3 alpha = Cross(dir, edge2);
		float det = Dot(alpha, edge1);
		// ゼロ除算対策
		det >= 0 ? det = Max(det, EPSILON) : det = Min(det, -EPSILON);

		Vector3 beta = Cross(originDir, edge1);
		float invDet = 1.f / det;
		// tを求める
		float t = invDet * Dot(beta, edge2);
		if (t < 0.f) {
			return INTERSECTION_TYPE::NONE;
		}
		// Uを求める
		float u = invDet * Dot(alpha, originDir);
		if (u < 0.f || u > 1.f) {
			return INTERSECTION_TYPE::NONE;
		}
		// Vを求める
		float v = invDet * Dot(beta, dir);
		if (v < 0.f || u + v > 1.f) {
			return INTERSECTION_TYPE::NONE;
		}

		// 交点あり
		enlarge = t;
		barycentricU = u;
		barycentricV = v;
		return INTERSECTION_TYPE::HIT;
	}
}

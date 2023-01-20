#include "Intersector.hpp"
#include "IntersectionResult.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "../Scene/Scene.hpp"

namespace PathTracer {
	Intersector::Intersector()
	{
	}
	Intersector::~Intersector()
	{
	}

	IntersectionResult Intersector::Intersect(const Ray& ray, const Scene& scene)
	{
		float minDistance = FLT_MAX;
		IntersectionResult intersectResult = IntersectionResult();
		for (auto mesh : scene.GetMeshes()) {
			IntersectionResult result = mesh->Intersect(ray);
			if (result.GetType() == INTERSECTION_TYPE::NONE) continue;
			if (result.GetDistance() < minDistance) {
				minDistance = result.GetDistance();
				intersectResult = result;
			}
		}
		return intersectResult;
	}

	IntersectionResult Intersector::IntersectTriangles(const Ray& ray, const Scene& scene)
	{
		float minDistance = FLT_MAX;
		IntersectionResult intersectResult = IntersectionResult();
		for (auto mesh : scene.GetMeshes()) {
			std::vector<Vector3> vertices(mesh->GetVertices().size());

			Matrix4x4 modelMatrix = mesh->GetTransform().GetModelMatrix();

			// 座標変換
			for (size_t i = 0; i < vertices.size(); i++) {
				Vector4 pos = Vector4(mesh->GetVertices()[i], 1.f);
				vertices[i] = (modelMatrix * pos).xyz();
			}

			const std::vector<unsigned int>& indices = mesh->GetIndices();

			// メッシュの全ポリゴンに対して交差判定
			for (size_t i = 0; i < mesh->GetIndices().size(); i += 3) {
				// 三角形ポリゴンの頂点を取り出す
				Vector3 v0 = vertices[indices[i]];
				Vector3 v1 = vertices[indices[i + 1]];
				Vector3 v2 = vertices[indices[i + 2]];

				float t = 0.f;
				INTERSECTION_TYPE type = IntersectTriangle(ray, v0, v1, v2, t);

				if (type == INTERSECTION_TYPE::NONE) continue;

				float distance = (t * ray.GetDirection()).Length();
				if (distance < minDistance) {
					Vector3 pos = ray.GetOrigin() + t * ray.GetDirection();
					Vector3 normal = Normalize(Cross(v1 - v0, v2 - v0));
					minDistance = distance;
					intersectResult = IntersectionResult(pos, normal, distance, mesh->GetObjectID(), type);
				}
			}
		}
		return intersectResult;
	}

	IntersectionResult Intersector::Intersect(const Ray& ray, const Scene& scene, bool isExitOnceFound)
	{
		if (isExitOnceFound) {
			for (auto mesh : scene.GetMeshes()) {
				IntersectionResult result = mesh->Intersect(ray);
				if (result.GetType() == INTERSECTION_TYPE::HIT)
					return result;
			}
			return IntersectionResult();
		}
		else
			return Intersect(ray, scene);
	}

	INTERSECTION_TYPE Intersector::IntersectTriangle(const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2, float& enlarge)
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
		det > 0 ? Max(det, EPSILON) : Min(det, -EPSILON);

		float invDet = 1.f / det;
		Vector3 beta = Cross(originDir, edge1);
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
		return INTERSECTION_TYPE::HIT;
	}
}

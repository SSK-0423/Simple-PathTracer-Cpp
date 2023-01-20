#pragma once
#include "../Utility/PathTracerMath.hpp"
#include "IntersectionResult.hpp"

namespace PathTracer {
	class Ray;
	class Scene;
	class IntersectionResult;

	class Intersector {
	public:
		Intersector();
		~Intersector();

		IntersectionResult Intersect(const Ray& ray, const Scene& scene);
		IntersectionResult IntersectTriangles(const Ray& ray, const Scene& scene);
		IntersectionResult Intersect(const Ray& ray, const Scene& scene, bool isExitOnceFound);

	private:
		INTERSECTION_TYPE IntersectTriangle(const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2, float& enlarge);
	};
}
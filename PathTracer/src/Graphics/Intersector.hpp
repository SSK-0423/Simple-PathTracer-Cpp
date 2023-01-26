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

		IntersectionResult IntersectTriangles(const Ray& ray, const Scene& scene);
	private:
		INTERSECTION_TYPE IntersectTriangle(const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2, float& enlarge, float& barycentricU, float& barycentricV);
	};
}
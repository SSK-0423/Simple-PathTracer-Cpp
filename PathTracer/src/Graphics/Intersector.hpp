#pragma once
#include "../Utility/PathTracerMath.hpp"
#include "IntersectionResult.hpp"
#include "Polygon.hpp"

namespace PathTracer {
	class Ray;
	class Scene;
	class BVH;
	class AABB;
	class IntersectionResult;

	class Intersector {
	public:
		Intersector();
		~Intersector();

		IntersectionResult Intersect(const Ray& ray, const Scene& scene, bool isWireFrame = false, bool exitOnceFound = false);
		IntersectionResult IntersectBVH(const Ray& ray, const Scene& scene, bool exitOnceFound = false);
		IntersectionResult IntersectBVH(const Ray& ray, const std::vector<AABB>& bvhNodes, int nodeIndex, bool isWireFrame = false, bool exitOnceFound = false);
		IntersectionResult IntersctPolygons(const Ray& ray, const std::vector<Polygon>& polygons, bool isWireFrame = false, bool exitOnceFound = false);
		INTERSECTION_TYPE IntersectTriangle(const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2, float& enlarge, float& barycentricU, float& barycentricV);
	};
}
#include "Intersector.hpp"
#include "IntersectionResult.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "../Scene/Scene.hpp"
#include "../Utility/PathTracerMath.hpp"

PathTracer::Intersector::Intersector()
{
}
PathTracer::Intersector::~Intersector()
{
}

PathTracer::IntersectionResult PathTracer::Intersector::Intersect(const Ray& ray, const Scene& scene)
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
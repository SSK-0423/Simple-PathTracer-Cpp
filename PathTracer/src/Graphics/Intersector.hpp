#pragma once
namespace PathTracer {
	class Ray;
	class Scene;
	class IntersectionResult;

	class Intersector {
	public:
		Intersector();
		~Intersector();

		IntersectionResult Intersect(const Ray& ray, const Scene& scene);
	private:
		IntersectionResult IntersectSphere(const Ray& ray, const Scene& scene);
	};
}
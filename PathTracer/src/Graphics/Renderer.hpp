#pragma once
#include "PixelBuffer.hpp"
#include "Intersector.hpp"
#include "../Scene/Camera.hpp"

namespace PathTracer {
	class Ray;
	class Scene;
	class Material;

	class Renderer {
	public:
		Renderer();
		~Renderer();

		void Init(const unsigned int& width, const unsigned int& height, const unsigned int& sampleCount);

		void Render(const Scene& scene);
	private:
		unsigned int m_width;
		unsigned int m_height;
		PixelBuffer m_renderTarget;
		Intersector m_intersector;

		Camera m_camera;
		unsigned int m_sampleCount;

		const Vector3 RayTrace(const Ray& ray, const Scene& scene, unsigned int bounce);
		const Vector3 RayTraceBVH(const Ray& ray, const Scene& scene, unsigned int bounce);
		const Vector3 RayTraceNEE(const Ray& ray, const Scene& scene, unsigned int bounce);
		const Vector3 RayTraceNEE(const Ray& ray, const Scene& scene, Vector3 weight, unsigned int bounce);
		const Vector3 RayTraceNEE(const Ray& ray, const Scene& scene);
		const Vector3 RayTraceNEEBVH(const Ray& ray, const Scene& scene);
		const Vector3 SamplePointOnHemisphere(const Vector3& surfaceNormal);
		const bool RussianRoulette(const Material& material);
	};
}
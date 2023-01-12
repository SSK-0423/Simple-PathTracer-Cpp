#include "Plane.hpp"
#include "Ray.hpp"
#include "IntersectionResult.hpp"
#include <float.h>

namespace PathTracer {
	const IntersectionResult& PathTracer::Plane::Intersect(const Ray& ray)
	{
		float DdotN = Dot(ray.GetDirection(), m_normal);
		// •ª•ê0‚ÍŒð“_‚È‚µ
		if (DdotN == 0)
			return IntersectionResult(Vector3(0.f, 0.f, 0.f), FLT_MAX, -1, INTERSECTION_TYPE::NONE);

		float t = Dot(m_position - ray.GetOrigin(), m_normal) / DdotN;

		// Œð“_‚ ‚è
		if (t > 0)
		{
			return IntersectionResult(
				ray.GetOrigin() + ray.GetDirection() * t,
				(ray.GetDirection() * t).Length(),
				m_objectID, INTERSECTION_TYPE::HIT);
		}
		// Œð“_‚È‚µ
		else
		{
			return IntersectionResult(Vector3(0.f, 0.f, 0.f), FLT_MAX, -1, INTERSECTION_TYPE::NONE);
		}
	}
}

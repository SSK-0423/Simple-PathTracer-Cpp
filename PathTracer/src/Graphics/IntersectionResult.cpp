#include "IntersectionResult.hpp"
#include "../Graphics/Polygon.hpp"
#include <float.h>

PathTracer::IntersectionResult::IntersectionResult()
	: m_position(0.f, 0.f, 0.f), m_distance(FLT_MAX), m_objectID(-1), m_type(INTERSECTION_TYPE::NONE), m_triangleMask(TRIANGLE_MASK::NONE)
{
}
PathTracer::IntersectionResult::IntersectionResult(
	const Vector3& position, const Vector3& normal, const float& distance, const unsigned int& objectID, const TRIANGLE_MASK& triangleMask, const PathTracer::INTERSECTION_TYPE& type)
	: m_position(position), m_normal(normal), m_distance(distance), m_objectID(objectID), m_triangleMask(m_triangleMask), m_type(type)
{
}
PathTracer::IntersectionResult::~IntersectionResult()
{
}

#include "IntersectionResult.hpp"
#include <float.h>

PathTracer::IntersectionResult::IntersectionResult()
	: m_position(0.f, 0.f, 0.f), m_distance(FLT_MAX), m_objectID(-1), m_type(INTERSECTION_TYPE::NONE)
{
}
PathTracer::IntersectionResult::IntersectionResult(const Vector3& position, const float& distance, const unsigned int& objectID, const PathTracer::INTERSECTION_TYPE& type)
	: m_position(position), m_distance(distance), m_objectID(objectID), m_type(type)
{
}
PathTracer::IntersectionResult::~IntersectionResult()
{
}

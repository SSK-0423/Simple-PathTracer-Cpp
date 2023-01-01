#include "IntersectionResult.hpp"
PathTracer::IntersectionResult::IntersectionResult()
{
}
PathTracer::IntersectionResult::IntersectionResult(const Vector3& position, const float& distance, const unsigned int& objectID, const INTERSECTION_TYPE& type)
	: position(position), distance(distance), objectID(objectID), type(type)
{
}
PathTracer::IntersectionResult::~IntersectionResult()
{
}

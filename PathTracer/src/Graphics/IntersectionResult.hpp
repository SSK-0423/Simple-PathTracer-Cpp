#pragma once
#include "../Utility/Vector.hpp"

namespace PathTracer {
	enum class INTERSECTION_TYPE {
		HIT = 0,
		NONE = -1,
	};

	class IntersectionResult {
	public:
		IntersectionResult();
		IntersectionResult(
			const Vector3& position, const Vector3& normal, const float& distance,
			const unsigned int& objectID, const INTERSECTION_TYPE& type);
		~IntersectionResult();

		const Vector3& GetPosition() { return m_position; }
		const Vector3& GetNormal() { return m_normal; }
		const float& GetDistance() { return m_distance; }
		const unsigned int& GetObjectID() { return m_objectID; }
		const INTERSECTION_TYPE& GetType() { return m_type; }

	private:
		Vector3 m_position;
		Vector3 m_normal;
		float m_distance;
		unsigned int m_objectID;
		INTERSECTION_TYPE m_type;
	};

}
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
			const Vector3& position, const float& distance, 
			const unsigned int& objectID, const INTERSECTION_TYPE& type);
		~IntersectionResult();

		INTERSECTION_TYPE GetType() { return type; }

	private:
		Vector3 position;
		float distance;
		unsigned int objectID;
		INTERSECTION_TYPE type;
	};

}
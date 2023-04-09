#pragma once
#include "../Utility/Vector.hpp"

namespace PathTracer {

	enum class LIGHT_TYPE {
		DIRECTIONAL,
		POINT,
		SPOT,
		AREA,
	};

	struct LightInfo {
		Vector3 direction;
		float distance;
		Vector3 color;
		float pdf;
		Vector3 normal;
		LIGHT_TYPE type;
	};

	class LightSource {
	public:
		LightSource() = default;
		virtual ~LightSource() = default;

		virtual const LightInfo Sample(const Vector3& position) = 0;
	};
}
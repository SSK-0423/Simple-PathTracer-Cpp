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
		LIGHT_TYPE type;
	};

	class LightSource {
	public:
		LightSource() = default;
		virtual ~LightSource() = default;

		virtual const LightInfo GetLightInfo(const Vector3& position) = 0;
	};
}
#pragma once
#include "LightSource.hpp"

namespace PathTracer {
	class DirectionalLight : public LightSource {
	public:
		DirectionalLight(const Vector3& direction, const Vector3& color)
			: m_direction(Normalize(direction)), m_color(color), m_type(LIGHT_TYPE::DIRECTIONAL) {}
		~DirectionalLight() = default;

		const LightInfo Sample(const Vector3& position) override
		{
			LightInfo info;
			info.color = m_color;
			info.direction = m_direction;
			info.distance = FLT_MAX;
			info.type = m_type;

			return info;
		}

	private:
		Vector3 m_color;
		Vector3 m_direction;
		LIGHT_TYPE m_type;
	};
}
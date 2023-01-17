#pragma once
#include "LightSource.hpp"

namespace PathTracer {
	class PointLight : public LightSource {
	public:
		PointLight(const Vector3& position, const Vector3& color)
			: m_position(position), m_color(color), m_type(LIGHT_TYPE::POINT) {}
		~PointLight() = default;

		const LightInfo GetLightInfo(const Vector3& position) override
		{
			LightInfo info;
			info.color = m_color;
			info.direction = Normalize(m_position - position);
			info.distance = (m_position - position).Length();
			info.type = m_type;

			return info;
		}
	private:
		Vector3 m_color;
		Vector3 m_position;
		LIGHT_TYPE m_type;
	};
}
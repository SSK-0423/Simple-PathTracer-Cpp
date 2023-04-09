#pragma once
#include "LightSource.hpp"
#include "Transform.hpp"

namespace PathTracer {
	class AreaLight : public LightSource {
	public:
		AreaLight(const Vector3& color, const Transform& transform) : m_color(color), m_transform(transform) {}
		~AreaLight() = default;

		const LightInfo Sample(const Vector3& position) override;
	private:
		Vector3 m_color;
		Transform m_transform;
	};
}
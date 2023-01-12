#pragma once

#include "../Utility/Vector.hpp"

namespace PathTracer {
	class Material {
	public:
		Material() : m_baseColor(Vector3(1.f, 1.f, 1.f)), m_metallic(0.f), m_roughness(0.f) {}
		Material(const Vector3& baseColor, const float& metallic = 0.f, const float& roughness = 0.f)
			: m_baseColor(baseColor), m_metallic(metallic), m_roughness(roughness) {}
		~Material() = default;

		const Vector3& GetBaseColor() const { return m_baseColor; }
		const float& GetMetallic() const { return m_metallic; }
		const float& GetRoughness() const { return m_roughness; }
	private:
		Vector3 m_baseColor;
		float m_metallic;
		float m_roughness;
	};
}
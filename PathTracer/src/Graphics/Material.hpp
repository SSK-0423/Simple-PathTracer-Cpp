#pragma once

#include "../Utility/Vector.hpp"

namespace PathTracer {
	class Material {
	public:
		Material() : m_baseColor(Vector3(1.f, 1.f, 1.f)), m_metallic(0.f), m_roughness(0.f), m_emittedColor(0.f, 0.f, 0.f), m_isReflective(false) {}
		Material(
			const Vector3& baseColor, const float& metallic = 0.f, const float& roughness = 0.f,
			const Vector3& emittedColor = Vector3(0.f, 0.f, 0.f), const bool& isReflective = false)
			: m_baseColor(baseColor), m_metallic(metallic), m_roughness(roughness), m_emittedColor(emittedColor), m_isReflective(isReflective) {}
		~Material() = default;

		const Vector3& GetBaseColor() const { return m_baseColor; }
		const float& GetMetallic() const { return m_metallic; }
		const float& GetRoughness() const { return m_roughness; }
		const Vector3& GetEmittedColor() const { return m_emittedColor; }
		const bool& isReflective() const { return m_isReflective; }
	private:
		Vector3 m_baseColor;
		Vector3 m_emittedColor;
		float m_metallic;
		float m_roughness;
		bool m_isReflective;
	};
}
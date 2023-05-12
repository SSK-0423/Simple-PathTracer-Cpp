#pragma once

#include "../Utility/Vector.hpp"

namespace PathTracer {

	class Material {
	public:
		Material()
			: m_baseColor(Vector3(1.f, 1.f, 1.f)), m_metallic(0.f), m_roughness(0.f), m_ior(1.f), m_emittedColor(0.f, 0.f, 0.f), m_isReflective(false), m_isRefractive(false) {}
		Material(
			const Vector3& baseColor, const float& metallic = 0.f, const float& roughness = 0.f,
			const Vector3& emittedColor = Vector3(0.f, 0.f, 0.f), const bool& isReflective = false, const bool& isRefractive = false, const float& ior = 1.f)
			: m_baseColor(baseColor), m_metallic(metallic), m_roughness(roughness), m_ior(ior), m_emittedColor(emittedColor),
			m_isReflective(isReflective), m_isRefractive(isRefractive) {}
		~Material() = default;

		const Vector3& GetBaseColor() const { return m_baseColor; }
		const float& GetMetallic() const { return m_metallic; }
		const float& GetRoughness() const { return m_roughness; }
		const float& GetIOR() const { return m_ior; }
		const Vector3& GetEmittedColor() const { return m_emittedColor; }
		const bool& isReflective() const { return m_isReflective; }
		const bool& isRefractive() const { return m_isRefractive; }
	private:
		Vector3 m_baseColor;
		Vector3 m_emittedColor;
		float m_metallic;
		float m_roughness;
		float m_ior; // ã¸ê‹ó¶
		bool m_isReflective;
		bool m_isRefractive;
	};
}
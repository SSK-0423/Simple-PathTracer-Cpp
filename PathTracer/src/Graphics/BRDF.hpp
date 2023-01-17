#pragma once
#include "../Utility/Vector.hpp"

namespace PathTracer {
	class Material;

	class DiffuseBRDF {
	public:
		static Vector3 NormalizeLambert(const Vector3& baseColor);
	};

	class SpecularBRDF {
	public:
		static Vector3 CookTorrance(const Material& material, const Vector3& N, const Vector3& H, const Vector3& V, const Vector3& L);
		static Vector3 GGX(const Material& material, const Vector3& N, const Vector3& H, const Vector3& V, const Vector3& L);
	private:
		static Vector3 TorranceSparrow(const float& D, const float& G, const Vector3& Fr, const float& cos_o, const float& cos_i);
		
		// フレネル項
		static Vector3 SchlickFresnel(const Vector3& baseColor, const float& metallic, const float& VH);

		// 法線分布関数
		static float Beckman(const float& roughness, const float& VH);
		static float GGX(const float& roughness, const float& NH);

		// マイクロファセット
		static float Vcavity(const float& NH, const float& NV, const float& NL, const float& VH);
		static float Smith(const float& roughness, const float& NL, const float& NV);
	};
}
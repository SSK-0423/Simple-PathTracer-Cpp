#include "BRDF.hpp"
#include "Material.hpp"
#include "../Utility/PathTracerMath.hpp"

namespace PathTracer {
	Vector3 DiffuseBRDF::NormalizeLambert(const Vector3& baseColor)
	{
		return baseColor * (1.f / PI);
	}
	Vector3 SpecularBRDF::CookTorrance(const Material& material, const Vector3& N, const Vector3& H, const Vector3& V, const Vector3& L)
	{
		float NH = Saturate(Dot(N, H));
		float NV = Saturate(Dot(N, V));
		float NL = Saturate(Dot(N, L));
		float VH = Saturate(Dot(V, H));

		float D = Beckman(material.GetRoughness(), NH);
		float G2 = Vcavity(NH, NV, NL, VH);
		Vector3 Fr = SchlickFresnel(material.GetBaseColor(), material.GetMetallic(), VH);
		float cos_o = NL; // 出射方向のコサイン
		float cos_i = NV; // 入射方向のコサイン

		return TorranceSparrow(D, G2, Fr, cos_o, cos_i);
	}
	Vector3 SpecularBRDF::GGX(const Material& material, const Vector3& N, const Vector3& H, const Vector3& V, const Vector3& L)
	{
		float NH = Max(Saturate(Dot(N, H)), EPSILON);
		float NV = Max(Saturate(Dot(N, V)), EPSILON);
		float NL = Max(Saturate(Dot(N, L)), EPSILON);
		float VH = Max(Saturate(Dot(V, H)), EPSILON);

		float D = GGX(material.GetRoughness(), NH);
		float G2 = Smith(material.GetRoughness(), NL, NV);
		Vector3 Fr = SchlickFresnel(material.GetBaseColor(), material.GetMetallic(), VH);
		float cos_o = NL; // 出射方向のコサイン
		float cos_i = NV; // 入射方向のコサイン

		return TorranceSparrow(D, G2, Fr, cos_o, cos_i);
	}
	Vector3 SpecularBRDF::TorranceSparrow(const float& D, const float& G, const Vector3& Fr, const float& cos_o, const float& cos_i)
	{
		// DFG / 4cos_o cos_i
		return (Fr * D * G) / (4.f * cos_o * cos_i + EPSILON);
	}
	Vector3 SpecularBRDF::SchlickFresnel(const Vector3& baseColor, const float& metallic, const float& VH)
	{
		// Fr = F(0) + (1 - F(0))(1 - cosθ)^5
		Vector3 F0 = baseColor * metallic;

		return F0 + (Vector3(1.f, 1.f, 1.f) - F0) * powf((1.f - VH), 5);
	}
	float SpecularBRDF::Beckman(const float& roughness, const float& VH)
	{
		float alpha = powf(roughness, 2);
		alpha = Max(EPSILON, roughness);
		float alpha2 = powf(alpha, 2);
		float cos4 = powf(VH, 4);
		float tan_mn2 = (1.f - powf(VH, 2)) / powf(VH, 2);

		return (1.f / (alpha2 * cos4)) * exp(-tan_mn2 / alpha2);
	}
	float SpecularBRDF::GGX(const float& roughness, const float& NH)
	{
		float alpha = powf(roughness, 2);
		float alpha2 = powf(alpha, 2);
		float cos2 = powf(NH, 2);
		float temp = powf(cos2 * (alpha2 - 1.f) + 1, 2);

		return alpha2 / Max(EPSILON, PI * temp);
	}
	float SpecularBRDF::Vcavity(const float& NH, const float& NV, const float& NL, const float& VH)
	{
		// VHが0である可能性があるのでゼロ除算を防ぐために分母に微小な値を加算する
		return Min(1.f, Min(2.f * NH * NV / VH, 2.f * NH * NL / VH));
	}
	float SpecularBRDF::Smith(const float& roughness, const float& NL, const float& NV)
	{
		float cos2Out = NV * NV;
		float cos2In = NL * NL;

		float tan2In = (1.f + cos2In) / cos2In;
		float tan2Out = (1.f + cos2Out) / cos2Out;

		float alpha2In = 1.f / (powf(roughness, 2) * tan2In);
		float alpha2Out = 1.f / (powf(roughness, 2) * tan2Out);

		float lambdaIn = (-1.f + sqrtf(1.f + 1.f / alpha2In)) * 0.5f;
		float lambdaOut = (-1.f + sqrtf(1.f + 1.f / alpha2Out)) * 0.5f;

		return 1.f / (1.f + lambdaIn, lambdaOut);
	}
}
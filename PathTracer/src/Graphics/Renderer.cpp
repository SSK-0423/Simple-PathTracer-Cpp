#include "Renderer.hpp"
#include "IntersectionResult.hpp"
#include "Intersector.hpp"
#include "Mesh.hpp"
#include "BRDF.hpp"
#include "LightSource.hpp"

#include "../Utility/PathTracerMath.hpp"

#include <iostream>
#include "../Scene/Scene.hpp"

constexpr unsigned int MAX_BOUNCE = 3;

PathTracer::Renderer::Renderer()
{
	float val[4][4] = { {1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16} };

	Matrix4x4 mat;
	mat.Show();

	mat = Matrix4x4::Identity();
	mat.Show();

	mat = Matrix4x4(Vector4(1, 0, 0, 0), Vector4(0, 1, 0, 0), Vector4(0, 0, 1, 0), Vector4(10, 20, 30, 1));
	mat.Show();

	Matrix4x4 mat2 = Matrix4x4(Vector4(1, 0, 0, 0), Vector4(0, 1, 0, 0), Vector4(0, 0, 1, 0), Vector4(10, -20, -30, 1));

	(mat * mat2).Show();

	Vector4 origin = Vector4(-5.f, 5.f, 0.f, 1.f);

	(mat * origin).Show();
}

PathTracer::Renderer::~Renderer()
{
}

void PathTracer::Renderer::Init(const unsigned int& width, const unsigned int& height, const unsigned int& sampleCount)
{
	m_width = width;
	m_height = height;
	m_sampleCount = sampleCount;
	m_renderTarget.Create(width, height);

	m_camera = Camera(Vector3(0, 0, -3.38), Vector3(0, 0, 1), Vector3(1, 0, 0), Vector3(0, 1, 0), 45.f, width, height);
}

void PathTracer::Renderer::Render(const Scene& scene)
{
	// パストレーシング
	for (size_t y = 0; y < m_height; y++) {
		for (size_t x = 0; x < m_width; x++) {
			Vector3 accumulatedRadiance = Vector3(0.f, 0.f, 0.f);
			Ray cameraRay = m_camera.GetCameraRay(x, y, m_width, m_height);

			IntersectionResult result = m_intersector.IntersectTriangles(cameraRay, scene);

			if (result.GetType() == INTERSECTION_TYPE::HIT) {
				m_renderTarget.Write(x, y, 1.f, 1.f, 1.f);
			}
			else {
				m_renderTarget.Write(x, y, 0.f, 1.f, 1.f);
			}

			//for (size_t s = 0; s < m_sampleCount; s++) {
			//	accumulatedRadiance += RayTrace(cameraRay, scene, 0);
			//}
			//accumulatedRadiance = accumulatedRadiance / m_sampleCount;
			//m_renderTarget.Write(x, y, accumulatedRadiance.r(), accumulatedRadiance.g(), accumulatedRadiance.b());
		}
	}

	// パストレーシング結果を出力
	m_renderTarget.OutputImage("CornellBox.ppm");
}

const Vector3 PathTracer::Renderer::RayTrace(const Ray& ray, const Scene& scene, unsigned int bounce)
{
	if (bounce > MAX_BOUNCE) {
		return Vector3(0.f, 0.f, 0.f);
	}

	// シーンとレイの交差判定
	IntersectionResult result = m_intersector.Intersect(ray, scene);

	// シェーディング
	if (result.GetType() == INTERSECTION_TYPE::HIT) {
		// マテリアル
		Material material = scene.GetMesh(result.GetObjectID())->GetMaterial();


		// 半球方向の1点をサンプリング
		Vector3 surfaceNormal = result.GetNormal();
		Vector3 incidentDirection = SamplePointOnHemisphere(surfaceNormal);
		Vector3 viewDir = Normalize(-1.f * ray.GetDirection());	// 出射方向はRayの逆ベクトル
		Vector3 halfVector = Normalize(surfaceNormal + incidentDirection);

		// シェーディング
		Vector3 diffuseBRDF = DiffuseBRDF::NormalizeLambert(material.GetBaseColor()) * (1.f - material.GetMetallic());
		Vector3 emittion = material.GetEmittedColor();

		// 光源なら寄与計算終了
		if (emittion.r() > 0.f || emittion.g() > 0.f || emittion.b() > 0.f) {
			return material.GetEmittedColor();
		}

		// 半球方向の1点をサンプリングする
		// 全立体角が4πで半球についてのみ考えるので確率密度関数pdfは 1 / 2π
		// ここをBRDFに従う形にすれば分散を低減できる
		float pdf = 1.f / (2.f * PI);

		// ロシアンルーレット
		std::random_device device;
		std::mt19937_64 mt(device());
		std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
		float random = randamGenerator(mt);


		// 反射確率の最大値を0.5とする
		float albedo = 1.f - material.GetMetallic();
		// 拡散反射
		if (random < albedo) {
			// 新しいレイを生成
			Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, incidentDirection);
			// Li(x,ω)
			Vector3 incidentLight = diffuseBRDF * RayTrace(newRay, scene, ++bounce);
			// Lo = Le(x,ω') + Li(x,ω) * BRDF * cosθ / pdf
			return emittion + incidentLight * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / albedo);
		}
		// 鏡面反射
		else if (random >= albedo && random < albedo + material.GetMetallic()) {
			// 新しいレイを生成
			// TODO: 鏡面反射BRDFに基づいた重点的サンプリング
			// 今はとりあえず完全鏡面反射のみを考える
			Vector3 reflectDir = Normalize(2.f * Dot(viewDir, surfaceNormal) * surfaceNormal - viewDir);
			Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
			// Li(x,ω)
			Vector3 incidentLight = RayTrace(newRay, scene, ++bounce);
			// Lo = Le(x,ω') + Li(x,ω) * BRDF * cosθ / pdf
			return emittion + incidentLight * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / material.GetMetallic());
		}
		else {
			return emittion;
		}
	}

	return Vector3(0.f, 0.f, 0.f);
}

const Vector3 PathTracer::Renderer::SamplePointOnHemisphere(const Vector3& surfaceNormal)
{
	// 一様乱数を生成
	std::random_device device;
	std::mt19937_64 mt(device());
	std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
	float u1 = randamGenerator(mt);
	float u2 = randamGenerator(mt);

	// Z軸を頂点方向として半球上の１点をサンプル
	float phi = 2.f * PI * u1;
	float x = sqrtf(u2) * cosf(phi);
	float y = sqrtf(u2) * sinf(phi);
	float z = sqrtf(1.f - u2);

	// 物体表面上の法線に合わせて基底変換
	// 1. 物体表面上の法線を含めた基底を計算する
	Vector3 w, u, v;
	w = surfaceNormal;
	// 法線方向がY軸(0,1,0)と平行な場合は基底ベクトルの算出にY軸を使用できないため、
	// X軸を使用して基底ベクトルを算出する
	// 法線のx値が限りなく0に近い場合はY軸と平行であるとみなす
	if (fabs(w.x) < EPSILON) {
		u = Normalize(Cross(Vector3(1.f, 0.f, 0.f), w));
	}
	else {
		u = Normalize(Cross(Vector3(0.f, 1.f, 0.f), w));
	}
	v = Cross(w, u);

	return Normalize(u * x + v * y + w * z);
}

const bool PathTracer::Renderer::RussianRoulette(const Material& material)
{
	// 一様乱数を生成
	std::random_device device;
	std::mt19937_64 mt(device());
	std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
	float random = randamGenerator(mt);
	// 反射確率の最大値を0.5とする
	float albedo = Min(1.f - material.GetMetallic(), 0.5f);
	if (random < albedo) {
		return true;
	}
	else {
		return false;
	}
}
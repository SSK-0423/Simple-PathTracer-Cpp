#include "Renderer.hpp"
#include "IntersectionResult.hpp"
#include "Intersector.hpp"
#include "Mesh.hpp"
#include "BRDF.hpp"
#include "LightSource.hpp"

#include "../Utility/PathTracerMath.hpp"

#include <iostream>
#include <time.h>
#include "../Scene/Scene.hpp"
#include "../Scene/BVH.hpp"

constexpr unsigned int MINIMUM_BOUNCE = 3;
constexpr unsigned int BOUNCE_LIMIT = 3;
constexpr clock_t RENDER_TIME_LIMIT = 1000 * 60 * 10 * 1;

PathTracer::Renderer::Renderer()
{

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
	unsigned int maxProgress = m_height * m_width * m_sampleCount;
	unsigned int progress = 0;

	auto bvhNodes = scene.GetBVH().GetNodes();
	auto nodesCount = bvhNodes.size();
	auto pixelCount = m_height * m_width;

	clock_t start = clock();
	bool isEnd = false;

	std::vector<Vector3> accumulatedRadiance(m_height * m_width);
	std::vector<unsigned int> sampledCount(m_height * m_width, 0);

	printf("\n---------レンダリング開始---------\n");
	printf("\n");

	// 一様乱数を生成
	std::random_device device;
	std::mt19937_64 mt(device());
	std::uniform_real_distribution<> randamGenerator(0.f, 1.f);

	// パストレーシング
	while (true)
	{
#pragma omp parallel for
		for (int y = 0; y < m_height; y++) {
			for (int x = 0; x < m_width; x++) {
				float u = randamGenerator(mt);
				float v = randamGenerator(mt);
				Ray cameraRay = m_camera.GetCameraRay(x + u, y + v, m_width, m_height);

				accumulatedRadiance[y * m_width + x] += RayTraceBVH(cameraRay, scene, 0);
				//accumulatedRadiance[y * m_width + x] += RayTraceNEEBVH(cameraRay, scene);
				sampledCount[y * m_width + x] += 1;

				clock_t now = clock();
				//printf("\rRemaining Time(ms) %d", RENDER_TIME_LIMIT - (now - start));

				if (now - start > RENDER_TIME_LIMIT) {
					isEnd = true; break;
				}
			}
			if (isEnd) break;
		}
		if (isEnd) break;
	}


	//#pragma omp parallel for
	//	for (int s = 0; s < m_sampleCount; s++) {
	//		for (int y = 0; y < m_height; y++) {
	//			for (int x = 0; x < m_width; x++) {
	//				float u = randamGenerator(mt);
	//				float v = randamGenerator(mt);
	//				Ray cameraRay = m_camera.GetCameraRay(x + u, y + v, m_width, m_height);
	//
	//				accumulatedRadiance[y * m_width + x] += RayTraceBVH(cameraRay, scene, 0);
	//
	//				sampledCount[y * m_width + x] += 1;
	//
	//				progress++;
	//				printf("\r%7.2f%%", static_cast<double>(progress) / static_cast<double>(maxProgress) * 100.f);
	//			}
	//		}
	//	}

	printf("\n");
	printf("\nレンダリング終了\n");
	printf("サンプリング数：%7d\n", std::min(sampledCount[0], sampledCount[m_width * m_height - 1]));

#pragma omp parallel for
	for (int y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++) {
			// 一度もレンダリングできていないなら飛ばす
			if (sampledCount[y * m_width + x] == 0) continue;

			Vector3 radiance = accumulatedRadiance[y * m_width + x] / sampledCount[y * m_width + x];

			m_renderTarget.Write(x, y, radiance.r(), radiance.g(), radiance.b());
		}
	}

	printf("\n");
	// パストレーシング結果を出力
	m_renderTarget.OutputImage("CornelBox.ppm");
}

const Vector3 PathTracer::Renderer::RayTrace(const Ray& ray, const Scene& scene, unsigned int bounce)
{
	if (bounce > BOUNCE_LIMIT)
		return Vector3(0.f, 0.f, 0.f);

	// シーンとレイの交差判定
	IntersectionResult result = m_intersector.Intersect(ray, scene);

	// シェーディング
	if (result.GetType() == INTERSECTION_TYPE::HIT) {

		// マテリアル
		Material material = scene.GetMesh(result.GetObjectID())->GetMaterial();

		// 光源に当たったなら寄与計算終了
		// triangleMaskを作成したがバグがあるのでemissionで判定
		if (material.GetEmittedColor().Length() > 0.f) {
			return material.GetEmittedColor();
		}

		// 半球方向の1点をサンプリング
		Vector3 surfaceNormal = result.GetNormal();
		Vector3 incidentDirection = SamplePointOnHemisphere(surfaceNormal);
		Vector3 viewDir = Normalize(-1.f * ray.GetDirection());	// 出射方向はRayの逆ベクトル
		Vector3 halfVector = Normalize(surfaceNormal + incidentDirection);

		// シェーディング
		Vector3 diffuseBRDF = DiffuseBRDF::NormalizeLambert(material.GetBaseColor()) * (1.f - material.GetMetallic());
		Vector3 emittion = material.GetEmittedColor();

		// 半球方向の1点をサンプリングする
		// 全立体角が4πで半球についてのみ考えるので確率密度関数pdfは 1 / 2π
		// ここをBRDFに従う形にすれば分散を低減できる
		float pdf = 1.f / (2.f * PI);

		// ロシアンルーレット
		std::random_device device;
		std::mt19937_64 mt(device());
		std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
		float random = randamGenerator(mt);

		float albedo = 1.f - material.GetMetallic();

		if (bounce > MINIMUM_BOUNCE) {
			albedo *= powf(EPSILON, bounce - MINIMUM_BOUNCE + 1);
		}

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
const Vector3 PathTracer::Renderer::RayTraceBVH(const Ray& ray, const Scene& scene, unsigned int bounce)
{
	if (bounce > BOUNCE_LIMIT)
		return Vector3(0.f, 0.f, 0.f);

	// シーンとレイの交差判定
	IntersectionResult result = m_intersector.IntersectBVH(ray, scene.GetBVH().GetNodes(), 0);

	// シェーディング
	if (result.GetType() == INTERSECTION_TYPE::HIT) {

		// マテリアル
		Material material = scene.GetMesh(result.GetObjectID())->GetMaterial();

		// 光源に当たったなら寄与計算終了
		// triangleMaskを作成したがバグがあるのでemissionで判定
		if (material.GetEmittedColor().Length() > 0.f) {
			return material.GetEmittedColor();
		}

		// 半球方向の1点をサンプリング
		Vector3 surfaceNormal = result.GetNormal();
		Vector3 incidentDirection = SamplePointOnHemisphere(surfaceNormal);
		Vector3 viewDir = Normalize(-1.f * ray.GetDirection());	// 出射方向はRayの逆ベクトル
		Vector3 halfVector = Normalize(surfaceNormal + incidentDirection);

		// シェーディング
		Vector3 diffuseBRDF = DiffuseBRDF::NormalizeLambert(material.GetBaseColor()) * (1.f - material.GetMetallic());
		Vector3 emittion = material.GetEmittedColor();

		// 半球方向の1点をサンプリングする
		// 全立体角が4πで半球についてのみ考えるので確率密度関数pdfは 1 / 2π
		// ここをBRDFに従う形にすれば分散を低減できる
		float pdf = 1.f / (2.f * PI);

		// ロシアンルーレット
		std::random_device device;
		std::mt19937_64 mt(device());
		std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
		float random = randamGenerator(mt);

		float albedo = 1.f - material.GetMetallic();

		if (bounce > MINIMUM_BOUNCE) {
			albedo *= powf(EPSILON, bounce - MINIMUM_BOUNCE + 1);
		}

		// 拡散反射
		if (random < albedo) {
			// 新しいレイを生成
			Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, incidentDirection);
			// Li(x,ω)
			Vector3 incidentLight = diffuseBRDF * RayTraceBVH(newRay, scene, ++bounce);
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
			Vector3 incidentLight = RayTraceBVH(newRay, scene, ++bounce);
			// Lo = Le(x,ω') + Li(x,ω) * BRDF * cosθ / pdf
			return emittion + incidentLight * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / material.GetMetallic());
		}
		else {
			return emittion;
		}
	}

	return Vector3(0.f, 0.f, 0.f);
}

const Vector3 PathTracer::Renderer::RayTraceNEE(const Ray& cameraRay, const Scene& scene)
{
	Vector3 radiance = Vector3(0.f, 0.f, 0.f);
	Vector3 alpha = Vector3(1.f, 1.f, 1.f);
	Ray ray = cameraRay;
	unsigned int bounce = 0;

	while (true)
	{
		if (bounce > BOUNCE_LIMIT) {
			break;
		}
		// シーンとレイの交差判定
		IntersectionResult result = m_intersector.Intersect(ray, scene);

		// シェーディング
		if (result.GetType() == INTERSECTION_TYPE::HIT) {
			// マテリアル
			Material material = scene.GetMesh(result.GetObjectID())->GetMaterial();

			if (material.GetEmittedColor().x > 0.f) {
				if (bounce == 0) {
					return material.GetEmittedColor();
				}
				break;
			}

			// 光源に当たったなら寄与計算終了(バグッてる)
			// triangleMaskが正しくセットされていなさそう
			//if (static_cast<TRIANGLE_MASK>(result.GetTriangleMask()) == TRIANGLE_MASK::LIGHT) {
			//	printf("LightHit bounce = %d\n", bounce);
			//	if (bounce == 0) {
			//		return material.GetEmittedColor();
			//	}
			//	break;
			//}

			// 半球方向の1点をサンプリング
			Vector3 surfaceNormal = result.GetNormal();
			Vector3 incidentDirection = SamplePointOnHemisphere(surfaceNormal);
			Vector3 viewDir = Normalize(-1.f * ray.GetDirection());	// 出射方向はRayの逆ベクトル
			Vector3 halfVector = Normalize(surfaceNormal + incidentDirection);

			Vector3 diffuseBRDF = DiffuseBRDF::NormalizeLambert(material.GetBaseColor()) * (1.f - material.GetMetallic());
			Vector3 emittion = material.GetEmittedColor();

			// ロシアンルーレット
			std::random_device device;
			std::mt19937_64 mt(device());
			std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
			float random = randamGenerator(mt);

			// 色の反射率の平均値をロシアンルーレットの闘値として扱う
			float albedo = 1.f - material.GetMetallic();

			if (bounce > MINIMUM_BOUNCE) {
				albedo *= powf(EPSILON, bounce - MINIMUM_BOUNCE);
			}

			// 拡散反射
			if (random < albedo) {
				// シャドウイング
				//光源上の１点をサンプルしてシャドウレイを生成
				auto light = scene.GetLightSources()[0];
				LightInfo lightInfo = light->Sample(result.GetPosition());
				Ray shadowRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, lightInfo.direction, lightInfo.distance - EPSILON);
				IntersectionResult shadowingResult = m_intersector.Intersect(shadowRay, scene, false, true);

				// 遮蔽されていない場合 ⇒直接光と関節光を考えることができる
				if (shadowingResult.GetType() == INTERSECTION_TYPE::NONE) {
					// G項 NEEでは必要
					float cos1 = std::abs(Dot(lightInfo.direction, result.GetNormal()));
					float cos2 = std::abs(Dot(-1.f * lightInfo.direction, lightInfo.normal));
					float G = cos1 * cos2 / powf(lightInfo.distance, 2.f);
					// Li(x,ω)
					radiance += alpha * (lightInfo.color * diffuseBRDF * G) / lightInfo.pdf;
				}
				// 新しいレイを生成
				ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, incidentDirection);

				// 半球方向の1点をサンプリングする
				// 全立体角が4πで半球についてのみ考えるので確率密度関数pdfは 1 / 2π
				// ここをBRDFに従う形にすれば分散を低減できる
				float pdf = 1.f / (2.f * PI);

				// Fr(ω,ω',x) * cosθ / pdf * (1 / Prr);
				alpha *= diffuseBRDF * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / albedo);
			}
			// 鏡面反射
			else if (random >= albedo && random < albedo + material.GetMetallic()) {
				// 新しいレイを生成
				// TODO: 鏡面反射BRDFに基づいた重点的サンプリング
				// 今はとりあえず完全鏡面反射のみを考える
				Vector3 reflectDir = Normalize(2.f * Dot(viewDir, surfaceNormal) * surfaceNormal - viewDir);
				ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
			}
		}
		else {
			break;
		}

		bounce++;
	}

	return radiance;
}

const Vector3 PathTracer::Renderer::RayTraceNEEBVH(const Ray& cameraRay, const Scene& scene)
{
	Vector3 radiance = Vector3(0.f, 0.f, 0.f);
	Vector3 alpha = Vector3(1.f, 1.f, 1.f);
	Ray ray = cameraRay;
	unsigned int bounce = 0;

	std::random_device device;
	std::mt19937_64 mt(device());
	std::uniform_real_distribution<> randamGenerator(0.f, 1.f);

	while (true)
	{
		if (bounce > BOUNCE_LIMIT) {
			break;
		}
		// シーンとレイの交差判定
		IntersectionResult result = m_intersector.IntersectBVH(ray, scene.GetBVH().GetNodes(), 0);

		// シェーディング
		if (result.GetType() == INTERSECTION_TYPE::HIT) {
			// マテリアル
			Material material = scene.GetMesh(result.GetObjectID())->GetMaterial();

			// 光源に当たったなら寄与計算終了
			// triangleMaskを作成したがバグがあるのでemissionで判定
			if (material.GetEmittedColor().Length() > 0.f) {
				if (bounce == 0) {
					return material.GetEmittedColor();
				}
				break;
			}

			// 半球方向の1点をサンプリング
			Vector3 surfaceNormal = result.GetNormal();
			Vector3 incidentDirection = SamplePointOnHemisphere(surfaceNormal);
			Vector3 viewDir = Normalize(-1.f * ray.GetDirection());	// 出射方向はRayの逆ベクトル
			Vector3 halfVector = Normalize(surfaceNormal + incidentDirection);

			Vector3 diffuseBRDF = DiffuseBRDF::NormalizeLambert(material.GetBaseColor()) * (1.f - material.GetMetallic());
			Vector3 emittion = material.GetEmittedColor();

			// ロシアンルーレット
			float random = randamGenerator(mt);

			// 色の反射率の平均値をロシアンルーレットの闘値として扱う
			float albedo = 1.f - material.GetMetallic();

			if (bounce > MINIMUM_BOUNCE) {
				albedo *= powf(EPSILON, bounce - MINIMUM_BOUNCE);
			}

			// 拡散反射
			if (random < albedo) {
				// シャドウイング
				//光源上の１点をサンプルしてシャドウレイを生成
				auto light = scene.GetLightSources()[0];
				LightInfo lightInfo = light->Sample(result.GetPosition());
				Ray shadowRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, lightInfo.direction, lightInfo.distance - EPSILON);
				IntersectionResult shadowingResult = m_intersector.IntersectBVH(shadowRay, scene.GetBVH().GetNodes(), 0, false, true);

				// 遮蔽されていない場合 ⇒直接光と関節光を考えることができる
				if (shadowingResult.GetType() == INTERSECTION_TYPE::NONE) {
					// G項 NEEでは必要
					float cos1 = std::abs(Dot(lightInfo.direction, result.GetNormal()));
					float cos2 = std::abs(Dot(-1.f * lightInfo.direction, lightInfo.normal));
					float G = cos1 * cos2 / powf(lightInfo.distance, 2.f);
					// Li(x,ω)
					radiance += emittion + alpha * (lightInfo.color * diffuseBRDF * G) / lightInfo.pdf * Saturate(Dot(surfaceNormal, lightInfo.direction));
				}
				// 新しいレイを生成
				ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, incidentDirection);

				// 半球方向の1点をサンプリングする
				// 全立体角が4πで半球についてのみ考えるので確率密度関数pdfは 1 / 2π
				// ここをBRDFに従う形にすれば分散を低減できる
				float pdf = 1.f / (2.f * PI);

				// Fr(ω,ω',x) * cosθ / pdf * (1 / Prr);
				alpha *= diffuseBRDF * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / albedo);
			}
			// 鏡面反射
			else if (random >= albedo && random < albedo + material.GetMetallic()) {
				// 新しいレイを生成
				// TODO: 鏡面反射BRDFに基づいた重点的サンプリング
				// 今はとりあえず完全鏡面反射のみを考える
				Vector3 reflectDir = Normalize(2.f * Dot(viewDir, surfaceNormal) * surfaceNormal - viewDir);
				ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
			}
			else {
				break;
			}
		}
		else {
			break;
		}

		bounce++;
	}

	return radiance;
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
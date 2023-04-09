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

	printf("\n---------�����_�����O�J�n---------\n");
	printf("\n");

	// ��l�����𐶐�
	std::random_device device;
	std::mt19937_64 mt(device());
	std::uniform_real_distribution<> randamGenerator(0.f, 1.f);

	// �p�X�g���[�V���O
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
	printf("\n�����_�����O�I��\n");
	printf("�T���v�����O���F%7d\n", std::min(sampledCount[0], sampledCount[m_width * m_height - 1]));

#pragma omp parallel for
	for (int y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++) {
			// ��x�������_�����O�ł��Ă��Ȃ��Ȃ��΂�
			if (sampledCount[y * m_width + x] == 0) continue;

			Vector3 radiance = accumulatedRadiance[y * m_width + x] / sampledCount[y * m_width + x];

			m_renderTarget.Write(x, y, radiance.r(), radiance.g(), radiance.b());
		}
	}

	printf("\n");
	// �p�X�g���[�V���O���ʂ��o��
	m_renderTarget.OutputImage("CornelBox.ppm");
}

const Vector3 PathTracer::Renderer::RayTrace(const Ray& ray, const Scene& scene, unsigned int bounce)
{
	if (bounce > BOUNCE_LIMIT)
		return Vector3(0.f, 0.f, 0.f);

	// �V�[���ƃ��C�̌�������
	IntersectionResult result = m_intersector.Intersect(ray, scene);

	// �V�F�[�f�B���O
	if (result.GetType() == INTERSECTION_TYPE::HIT) {

		// �}�e���A��
		Material material = scene.GetMesh(result.GetObjectID())->GetMaterial();

		// �����ɓ��������Ȃ��^�v�Z�I��
		// triangleMask���쐬�������o�O������̂�emission�Ŕ���
		if (material.GetEmittedColor().Length() > 0.f) {
			return material.GetEmittedColor();
		}

		// ����������1�_���T���v�����O
		Vector3 surfaceNormal = result.GetNormal();
		Vector3 incidentDirection = SamplePointOnHemisphere(surfaceNormal);
		Vector3 viewDir = Normalize(-1.f * ray.GetDirection());	// �o�˕�����Ray�̋t�x�N�g��
		Vector3 halfVector = Normalize(surfaceNormal + incidentDirection);

		// �V�F�[�f�B���O
		Vector3 diffuseBRDF = DiffuseBRDF::NormalizeLambert(material.GetBaseColor()) * (1.f - material.GetMetallic());
		Vector3 emittion = material.GetEmittedColor();

		// ����������1�_���T���v�����O����
		// �S���̊p��4�΂Ŕ����ɂ��Ă̂ݍl����̂Ŋm�����x�֐�pdf�� 1 / 2��
		// ������BRDF�ɏ]���`�ɂ���Ε��U��ጸ�ł���
		float pdf = 1.f / (2.f * PI);

		// ���V�A�����[���b�g
		std::random_device device;
		std::mt19937_64 mt(device());
		std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
		float random = randamGenerator(mt);

		float albedo = 1.f - material.GetMetallic();

		if (bounce > MINIMUM_BOUNCE) {
			albedo *= powf(EPSILON, bounce - MINIMUM_BOUNCE + 1);
		}

		// �g�U����
		if (random < albedo) {
			// �V�������C�𐶐�
			Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, incidentDirection);
			// Li(x,��)
			Vector3 incidentLight = diffuseBRDF * RayTrace(newRay, scene, ++bounce);
			// Lo = Le(x,��') + Li(x,��) * BRDF * cos�� / pdf
			return emittion + incidentLight * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / albedo);
		}
		// ���ʔ���
		else if (random >= albedo && random < albedo + material.GetMetallic()) {
			// �V�������C�𐶐�
			// TODO: ���ʔ���BRDF�Ɋ�Â����d�_�I�T���v�����O
			// ���͂Ƃ肠�������S���ʔ��˂݂̂��l����
			Vector3 reflectDir = Normalize(2.f * Dot(viewDir, surfaceNormal) * surfaceNormal - viewDir);
			Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
			// Li(x,��)
			Vector3 incidentLight = RayTrace(newRay, scene, ++bounce);
			// Lo = Le(x,��') + Li(x,��) * BRDF * cos�� / pdf
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

	// �V�[���ƃ��C�̌�������
	IntersectionResult result = m_intersector.IntersectBVH(ray, scene.GetBVH().GetNodes(), 0);

	// �V�F�[�f�B���O
	if (result.GetType() == INTERSECTION_TYPE::HIT) {

		// �}�e���A��
		Material material = scene.GetMesh(result.GetObjectID())->GetMaterial();

		// �����ɓ��������Ȃ��^�v�Z�I��
		// triangleMask���쐬�������o�O������̂�emission�Ŕ���
		if (material.GetEmittedColor().Length() > 0.f) {
			return material.GetEmittedColor();
		}

		// ����������1�_���T���v�����O
		Vector3 surfaceNormal = result.GetNormal();
		Vector3 incidentDirection = SamplePointOnHemisphere(surfaceNormal);
		Vector3 viewDir = Normalize(-1.f * ray.GetDirection());	// �o�˕�����Ray�̋t�x�N�g��
		Vector3 halfVector = Normalize(surfaceNormal + incidentDirection);

		// �V�F�[�f�B���O
		Vector3 diffuseBRDF = DiffuseBRDF::NormalizeLambert(material.GetBaseColor()) * (1.f - material.GetMetallic());
		Vector3 emittion = material.GetEmittedColor();

		// ����������1�_���T���v�����O����
		// �S���̊p��4�΂Ŕ����ɂ��Ă̂ݍl����̂Ŋm�����x�֐�pdf�� 1 / 2��
		// ������BRDF�ɏ]���`�ɂ���Ε��U��ጸ�ł���
		float pdf = 1.f / (2.f * PI);

		// ���V�A�����[���b�g
		std::random_device device;
		std::mt19937_64 mt(device());
		std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
		float random = randamGenerator(mt);

		float albedo = 1.f - material.GetMetallic();

		if (bounce > MINIMUM_BOUNCE) {
			albedo *= powf(EPSILON, bounce - MINIMUM_BOUNCE + 1);
		}

		// �g�U����
		if (random < albedo) {
			// �V�������C�𐶐�
			Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, incidentDirection);
			// Li(x,��)
			Vector3 incidentLight = diffuseBRDF * RayTraceBVH(newRay, scene, ++bounce);
			// Lo = Le(x,��') + Li(x,��) * BRDF * cos�� / pdf
			return emittion + incidentLight * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / albedo);
		}
		// ���ʔ���
		else if (random >= albedo && random < albedo + material.GetMetallic()) {
			// �V�������C�𐶐�
			// TODO: ���ʔ���BRDF�Ɋ�Â����d�_�I�T���v�����O
			// ���͂Ƃ肠�������S���ʔ��˂݂̂��l����
			Vector3 reflectDir = Normalize(2.f * Dot(viewDir, surfaceNormal) * surfaceNormal - viewDir);
			Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
			// Li(x,��)
			Vector3 incidentLight = RayTraceBVH(newRay, scene, ++bounce);
			// Lo = Le(x,��') + Li(x,��) * BRDF * cos�� / pdf
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
		// �V�[���ƃ��C�̌�������
		IntersectionResult result = m_intersector.Intersect(ray, scene);

		// �V�F�[�f�B���O
		if (result.GetType() == INTERSECTION_TYPE::HIT) {
			// �}�e���A��
			Material material = scene.GetMesh(result.GetObjectID())->GetMaterial();

			if (material.GetEmittedColor().x > 0.f) {
				if (bounce == 0) {
					return material.GetEmittedColor();
				}
				break;
			}

			// �����ɓ��������Ȃ��^�v�Z�I��(�o�O�b�Ă�)
			// triangleMask���������Z�b�g����Ă��Ȃ�����
			//if (static_cast<TRIANGLE_MASK>(result.GetTriangleMask()) == TRIANGLE_MASK::LIGHT) {
			//	printf("LightHit bounce = %d\n", bounce);
			//	if (bounce == 0) {
			//		return material.GetEmittedColor();
			//	}
			//	break;
			//}

			// ����������1�_���T���v�����O
			Vector3 surfaceNormal = result.GetNormal();
			Vector3 incidentDirection = SamplePointOnHemisphere(surfaceNormal);
			Vector3 viewDir = Normalize(-1.f * ray.GetDirection());	// �o�˕�����Ray�̋t�x�N�g��
			Vector3 halfVector = Normalize(surfaceNormal + incidentDirection);

			Vector3 diffuseBRDF = DiffuseBRDF::NormalizeLambert(material.GetBaseColor()) * (1.f - material.GetMetallic());
			Vector3 emittion = material.GetEmittedColor();

			// ���V�A�����[���b�g
			std::random_device device;
			std::mt19937_64 mt(device());
			std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
			float random = randamGenerator(mt);

			// �F�̔��˗��̕��ϒl�����V�A�����[���b�g�̓��l�Ƃ��Ĉ���
			float albedo = 1.f - material.GetMetallic();

			if (bounce > MINIMUM_BOUNCE) {
				albedo *= powf(EPSILON, bounce - MINIMUM_BOUNCE);
			}

			// �g�U����
			if (random < albedo) {
				// �V���h�E�C���O
				//������̂P�_���T���v�����ăV���h�E���C�𐶐�
				auto light = scene.GetLightSources()[0];
				LightInfo lightInfo = light->Sample(result.GetPosition());
				Ray shadowRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, lightInfo.direction, lightInfo.distance - EPSILON);
				IntersectionResult shadowingResult = m_intersector.Intersect(shadowRay, scene, false, true);

				// �Օ�����Ă��Ȃ��ꍇ �˒��ڌ��Ɗ֐ߌ����l���邱�Ƃ��ł���
				if (shadowingResult.GetType() == INTERSECTION_TYPE::NONE) {
					// G�� NEE�ł͕K�v
					float cos1 = std::abs(Dot(lightInfo.direction, result.GetNormal()));
					float cos2 = std::abs(Dot(-1.f * lightInfo.direction, lightInfo.normal));
					float G = cos1 * cos2 / powf(lightInfo.distance, 2.f);
					// Li(x,��)
					radiance += alpha * (lightInfo.color * diffuseBRDF * G) / lightInfo.pdf;
				}
				// �V�������C�𐶐�
				ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, incidentDirection);

				// ����������1�_���T���v�����O����
				// �S���̊p��4�΂Ŕ����ɂ��Ă̂ݍl����̂Ŋm�����x�֐�pdf�� 1 / 2��
				// ������BRDF�ɏ]���`�ɂ���Ε��U��ጸ�ł���
				float pdf = 1.f / (2.f * PI);

				// Fr(��,��',x) * cos�� / pdf * (1 / Prr);
				alpha *= diffuseBRDF * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / albedo);
			}
			// ���ʔ���
			else if (random >= albedo && random < albedo + material.GetMetallic()) {
				// �V�������C�𐶐�
				// TODO: ���ʔ���BRDF�Ɋ�Â����d�_�I�T���v�����O
				// ���͂Ƃ肠�������S���ʔ��˂݂̂��l����
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
		// �V�[���ƃ��C�̌�������
		IntersectionResult result = m_intersector.IntersectBVH(ray, scene.GetBVH().GetNodes(), 0);

		// �V�F�[�f�B���O
		if (result.GetType() == INTERSECTION_TYPE::HIT) {
			// �}�e���A��
			Material material = scene.GetMesh(result.GetObjectID())->GetMaterial();

			// �����ɓ��������Ȃ��^�v�Z�I��
			// triangleMask���쐬�������o�O������̂�emission�Ŕ���
			if (material.GetEmittedColor().Length() > 0.f) {
				if (bounce == 0) {
					return material.GetEmittedColor();
				}
				break;
			}

			// ����������1�_���T���v�����O
			Vector3 surfaceNormal = result.GetNormal();
			Vector3 incidentDirection = SamplePointOnHemisphere(surfaceNormal);
			Vector3 viewDir = Normalize(-1.f * ray.GetDirection());	// �o�˕�����Ray�̋t�x�N�g��
			Vector3 halfVector = Normalize(surfaceNormal + incidentDirection);

			Vector3 diffuseBRDF = DiffuseBRDF::NormalizeLambert(material.GetBaseColor()) * (1.f - material.GetMetallic());
			Vector3 emittion = material.GetEmittedColor();

			// ���V�A�����[���b�g
			float random = randamGenerator(mt);

			// �F�̔��˗��̕��ϒl�����V�A�����[���b�g�̓��l�Ƃ��Ĉ���
			float albedo = 1.f - material.GetMetallic();

			if (bounce > MINIMUM_BOUNCE) {
				albedo *= powf(EPSILON, bounce - MINIMUM_BOUNCE);
			}

			// �g�U����
			if (random < albedo) {
				// �V���h�E�C���O
				//������̂P�_���T���v�����ăV���h�E���C�𐶐�
				auto light = scene.GetLightSources()[0];
				LightInfo lightInfo = light->Sample(result.GetPosition());
				Ray shadowRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, lightInfo.direction, lightInfo.distance - EPSILON);
				IntersectionResult shadowingResult = m_intersector.IntersectBVH(shadowRay, scene.GetBVH().GetNodes(), 0, false, true);

				// �Օ�����Ă��Ȃ��ꍇ �˒��ڌ��Ɗ֐ߌ����l���邱�Ƃ��ł���
				if (shadowingResult.GetType() == INTERSECTION_TYPE::NONE) {
					// G�� NEE�ł͕K�v
					float cos1 = std::abs(Dot(lightInfo.direction, result.GetNormal()));
					float cos2 = std::abs(Dot(-1.f * lightInfo.direction, lightInfo.normal));
					float G = cos1 * cos2 / powf(lightInfo.distance, 2.f);
					// Li(x,��)
					radiance += emittion + alpha * (lightInfo.color * diffuseBRDF * G) / lightInfo.pdf * Saturate(Dot(surfaceNormal, lightInfo.direction));
				}
				// �V�������C�𐶐�
				ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, incidentDirection);

				// ����������1�_���T���v�����O����
				// �S���̊p��4�΂Ŕ����ɂ��Ă̂ݍl����̂Ŋm�����x�֐�pdf�� 1 / 2��
				// ������BRDF�ɏ]���`�ɂ���Ε��U��ጸ�ł���
				float pdf = 1.f / (2.f * PI);

				// Fr(��,��',x) * cos�� / pdf * (1 / Prr);
				alpha *= diffuseBRDF * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / albedo);
			}
			// ���ʔ���
			else if (random >= albedo && random < albedo + material.GetMetallic()) {
				// �V�������C�𐶐�
				// TODO: ���ʔ���BRDF�Ɋ�Â����d�_�I�T���v�����O
				// ���͂Ƃ肠�������S���ʔ��˂݂̂��l����
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
	// ��l�����𐶐�
	std::random_device device;
	std::mt19937_64 mt(device());
	std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
	float u1 = randamGenerator(mt);
	float u2 = randamGenerator(mt);

	// Z���𒸓_�����Ƃ��Ĕ�����̂P�_���T���v��
	float phi = 2.f * PI * u1;
	float x = sqrtf(u2) * cosf(phi);
	float y = sqrtf(u2) * sinf(phi);
	float z = sqrtf(1.f - u2);

	// ���̕\�ʏ�̖@���ɍ��킹�Ċ��ϊ�
	// 1. ���̕\�ʏ�̖@�����܂߂������v�Z����
	Vector3 w, u, v;
	w = surfaceNormal;
	// �@��������Y��(0,1,0)�ƕ��s�ȏꍇ�͊��x�N�g���̎Z�o��Y�����g�p�ł��Ȃ����߁A
	// X�����g�p���Ċ��x�N�g�����Z�o����
	// �@����x�l������Ȃ�0�ɋ߂��ꍇ��Y���ƕ��s�ł���Ƃ݂Ȃ�
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
	// ��l�����𐶐�
	std::random_device device;
	std::mt19937_64 mt(device());
	std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
	float random = randamGenerator(mt);
	// ���ˊm���̍ő�l��0.5�Ƃ���
	float albedo = Min(1.f - material.GetMetallic(), 0.5f);
	if (random < albedo) {
		return true;
	}
	else {
		return false;
	}
}
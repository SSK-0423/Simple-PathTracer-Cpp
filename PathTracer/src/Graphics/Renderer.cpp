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

//#define TIME_RENDERING

constexpr unsigned int MINIMUM_BOUNCE = 2;
constexpr unsigned int BOUNCE_LIMIT = 3;
constexpr clock_t RENDER_TIME_LIMIT = 1000 * 60 * 60 * 8;

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

	m_camera = Camera(Vector3(0, 0, -3.38 * 15.f), Vector3(0, 0, 1), Vector3(1, 0, 0), Vector3(0, 1, 0), 45.f, width, height);
}

void PathTracer::Renderer::Render(const Scene& scene, const std::string& outputPath)
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
#ifdef TIME_RENDERING
	while (true)
	{
#pragma omp parallel for
		for (int y = 0; y < m_height; y++) {
			for (int x = 0; x < m_width; x++) {
				float u = randamGenerator(mt);
				float v = randamGenerator(mt);
				Ray cameraRay = m_camera.GetCameraRay(x + u, y + v, m_width, m_height);

				Vector3 radiance = RayTraceNEEBVH(cameraRay, scene);
				// �L���Ȓl�݂̂����Z
				if (isfinite(radiance.r()) && isfinite(radiance.g()) && isfinite(radiance.b()))
				{
					accumulatedRadiance[y * m_width + x] += radiance / (radiance + 1.f);
					sampledCount[y * m_width + x] += 1;
				}

				clock_t now = clock();
				printf("\rRemaining Time(ms) %d", RENDER_TIME_LIMIT - (now - start));

				if (now - start > RENDER_TIME_LIMIT) {
					isEnd = true; break;
				}
			}
			if (isEnd) break;
		}
		if (isEnd) break;
	}
#else
#pragma omp parallel for
	for (int s = 0; s < m_sampleCount; s++) {
		for (int y = 0; y < m_height; y++) {
			for (int x = 0; x < m_width; x++) {
				float u = randamGenerator(mt);
				float v = randamGenerator(mt);
				Ray cameraRay = m_camera.GetCameraRay(x + u, y + v, m_width, m_height);

				Vector3 radiance = RayTraceNEEBVH(cameraRay, scene);
				// �L���Ȓl�݂̂����Z
				if (isfinite(radiance.r()) && isfinite(radiance.g()) && isfinite(radiance.b()))
				{
					accumulatedRadiance[y * m_width + x] += radiance / (radiance + 1.f);
					sampledCount[y * m_width + x] += 1;
				}
				progress++;
				printf("\r%7.2f%%", static_cast<double>(progress) / static_cast<double>(maxProgress) * 100.f);
			}
		}
	}
#endif // TIME_RENDERING

	printf("\n");
	printf("\n�����_�����O�I��\n");
	printf("�T���v�����O���F%7d\n", std::min(sampledCount[0], sampledCount[static_cast<size_t>(m_width) * m_height - 1]));

#pragma omp parallel for
	for (int y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++) {
			// ��x�������_�����O�ł��Ă��Ȃ��Ȃ��΂�
			if (sampledCount[y * m_width + x] == 0) continue;

			Vector3 radiance = accumulatedRadiance[y * m_width + x] / sampledCount[y * m_width + x];
			//radiance = radiance / (radiance + 1.f);

			m_renderTarget.Write(x, y, radiance.r(), radiance.g(), radiance.b());
		}
	}

	printf("\n");
	// �p�X�g���[�V���O���ʂ��o��
	m_renderTarget.OutputImage(outputPath);
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

		float russianRouletteProb = 1.f - material.GetMetallic();

		if (bounce > MINIMUM_BOUNCE) {
			russianRouletteProb *= powf(EPSILON, bounce - MINIMUM_BOUNCE + 1);
		}

		// �g�U����
		if (random < russianRouletteProb) {
			// �V�������C�𐶐�
			Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, incidentDirection);
			// Li(x,��)
			Vector3 incidentLight = diffuseBRDF * RayTrace(newRay, scene, bounce + 1);
			// Lo = Le(x,��') + Li(x,��) * BRDF * cos�� / pdf
			return emittion + incidentLight * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / russianRouletteProb);
		}
		// ���S���ʔ���
		else if (random >= russianRouletteProb && random < russianRouletteProb + material.GetMetallic()) {
			if (material.isReflective()) {
				// TODO: ���ʔ���BRDF�Ɋ�Â����d�_�I�T���v�����O
				Vector3 reflectDir = Reflect(viewDir, surfaceNormal);
				Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
				// Li(x,��)
				Vector3 incidentLight = RayTrace(newRay, scene, bounce + 1);
				// Lo = Le(x,��') + Li(x,��) * BRDF * cos�� / pdf
				// ���S���ʔ���BRDF�̒�` Fr(��)/cos�� Fr(��)�̓t���l�� �����ł͋��ʔ��˂������Ȃ��̂�Fr(��) = 1
				// �ɂ��,pdf = 1�ƂȂ�B�܂��A cos�Ƃ͑ł��������
				// ���S���ʔ��˂͓��ˊp�Ɣ��ˊp�����������߁A���e�ʐς��ς��Ȃ��̂ł��̂悤�Ȓ�`�ƂȂ�
				return emittion + incidentLight * (1.f - russianRouletteProb);

			}
			// ����
			if (material.isRefractive()) {

				// ���˃x�N�g��
				Vector3 reflectDir = Reflect(viewDir, surfaceNormal);
				Ray reflectionRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);

				// ���C���I�u�W�F�N�g�������̂��A�o��̂� ����Ftrue �o��Ffalse
				bool into = Dot(viewDir, surfaceNormal) > 0.f;

				const float nc = 1.f; // �^��̋��ܗ�
				const float ng = material.GetIOR(); // �K���X�̋��ܗ�
				const float nng = into ? nc / ng : ng / nc;

				// ���܃x�N�g��
				Vector3 refractDir = Refract(viewDir, surfaceNormal, nng, into);

				// �S���˂̏ꍇ�͔��ː����݂̂�ǐՂ���
				if (refractDir == Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX)) {
					return emittion + RayTrace(reflectionRay, scene, bounce + 1) * (1.f - russianRouletteProb);
				}
				Ray refractionRay = Ray(result.GetPosition() + surfaceNormal * EPSILON * (into ? -1.f : 1.f), refractDir);

				// Schlick��Frenel�ߎ���
				const float cos = into ? Dot(surfaceNormal, viewDir) : Dot(surfaceNormal * (into ? -1.f : 1.f), refractDir);

				// ���������S���ʔ��ˁE���S���܂�BSDF�ɊY������
				const float fr = SchlickFresnel(nc, ng, cos);	// ���ˌ��̊���
				// ���܃��C�̉^�ԕ��ˋP�x�͋��ܗ��̈قȂ镨�̂��ړ�����Ƃ��A���ܗ��̔�̓��ω�����
				const float ft = into ? powf(ng / nc, 2.f) : powf(nc / ng, 2.f);

				Vector3 incidentLight = Vector3();
				// ���C���w���I�ɑ�����̂�h�����߁A
				// ���ȏ�̃o�E���X�ȍ~�̓��V�A�����[���b�g�Ŕ��˂����܂ǂ��炩�݂̂�ǐՂ���
				const float prob = fr;
				if (bounce > 2) {
					// ����
					if (randamGenerator(mt) < prob) {
						// Li(x,��) * BRDF / prob
						incidentLight = RayTrace(reflectionRay, scene, bounce + 1) * fr / prob;
					}
					// ����
					else {
						// Li(x,��) * BRDF / prob
						incidentLight = RayTrace(refractionRay, scene, bounce + 1) * (1.f - fr) * ft / (1.f - prob);
					}
				}
				// ���˂Ƌ���
				else {
					incidentLight =
						RayTrace(reflectionRay, scene, bounce + 1) * fr +
						RayTrace(refractionRay, scene, bounce + 1) * (1.f - fr) * ft;
				}
				return emittion + incidentLight * (1.f - russianRouletteProb);
			}
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

		float russianRouletteProb = 1.f - material.GetMetallic();

		if (bounce > MINIMUM_BOUNCE) {
			russianRouletteProb *= powf(0.5, bounce - MINIMUM_BOUNCE);
		}

		// �g�U����
		if (random < russianRouletteProb) {
			// �V�������C�𐶐�
			Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, incidentDirection);
			// Li(x,��)
			Vector3 incidentLight = diffuseBRDF * RayTraceBVH(newRay, scene, bounce + 1);
			// Lo = Le(x,��') + Li(x,��) * BRDF * cos�� / pdf
			return emittion + incidentLight * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / russianRouletteProb);
		}
		// ���S���ʔ���
		else if (random >= russianRouletteProb && random < russianRouletteProb + material.GetMetallic()) {
			if (material.isReflective()) {
				// TODO: ���ʔ���BRDF�Ɋ�Â����d�_�I�T���v�����O
				Vector3 reflectDir = Reflect(viewDir, surfaceNormal);
				Ray newRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
				// Li(x,��)
				Vector3 incidentLight = RayTraceBVH(newRay, scene, bounce + 1);
				// Lo = Le(x,��') + Li(x,��) * BRDF * cos�� / pdf
				// ���S���ʔ���BRDF�̒�` Fr(��)/cos�� Fr(��)�̓t���l�� �����ł͋��ʔ��˂������Ȃ��̂�Fr(��) = 1
				// �ɂ��,pdf = 1�ƂȂ�B�܂��A cos�Ƃ͑ł��������
				// ���S���ʔ��˂͓��ˊp�Ɣ��ˊp�����������߁A���e�ʐς��ς��Ȃ��̂ł��̂悤�Ȓ�`�ƂȂ�
				return emittion + incidentLight * (1.f - russianRouletteProb);

			}
			// ����
			if (material.isRefractive()) {

				// ���˃x�N�g��
				Vector3 reflectDir = Reflect(viewDir, surfaceNormal);
				Ray reflectionRay = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);

				// ���C���I�u�W�F�N�g�������̂��A�o��̂� ����Ftrue �o��Ffalse
				bool into = Dot(viewDir, surfaceNormal) > 0.f;

				const float nc = 1.f; // �^��̋��ܗ�
				const float ng = material.GetIOR(); // �K���X�̋��ܗ�
				const float nng = into ? nc / ng : ng / nc;

				// ���܃x�N�g��
				Vector3 refractDir = Refract(viewDir, surfaceNormal, nng, into);

				// �S���˂̏ꍇ�͔��ː����݂̂�ǐՂ���
				if (refractDir == Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX)) {
					return emittion + RayTraceBVH(reflectionRay, scene, bounce + 1) * (1.f - russianRouletteProb);
				}
				Ray refractionRay = Ray(result.GetPosition() + surfaceNormal * EPSILON * (into ? -1.f : 1.f), refractDir);

				// Schlick��Frenel�ߎ���
				const float cos = into ? Dot(surfaceNormal, viewDir) : Dot(surfaceNormal * (into ? -1.f : 1.f), refractDir);

				// ���������S���ʔ��ˁE���S���܂�BSDF�ɊY������
				const float fr = SchlickFresnel(nc, ng, cos);	// ���ˌ��̊���
				// ���܃��C�̉^�ԕ��ˋP�x�͋��ܗ��̈قȂ镨�̂��ړ�����Ƃ��A���ܗ��̔�̓��ω�����
				const float ft = into ? powf(ng / nc, 2.f) : powf(nc / ng, 2.f);

				Vector3 incidentLight = Vector3();
				// ���C���w���I�ɑ�����̂�h�����߁A
				// ���ȏ�̃o�E���X�ȍ~�̓��V�A�����[���b�g�Ŕ��˂����܂ǂ��炩�݂̂�ǐՂ���
				const float prob = fr;
				if (bounce > 2) {
					// ����
					if (randamGenerator(mt) < prob) {
						// Li(x,��) * BRDF / prob
						incidentLight = RayTraceBVH(reflectionRay, scene, bounce + 1) * fr / prob;
					}
					// ����
					else {
						// Li(x,��) * BRDF / prob
						incidentLight = RayTraceBVH(refractionRay, scene, bounce + 1) * (1.f - fr) * ft / (1.f - prob);
					}
				}
				// ���˂Ƌ���
				else {
					incidentLight =
						RayTraceBVH(reflectionRay, scene, bounce + 1) * fr +
						RayTraceBVH(refractionRay, scene, bounce + 1) * (1.f - fr) * ft;
				}
				return emittion + incidentLight * (1.f - russianRouletteProb);
			}
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
	bool isReflected = false;

	std::random_device device;
	std::mt19937_64 mt(device());
	std::uniform_real_distribution<> randamGenerator(0.f, 1.f);

	while (true)
	{
		if (bounce > BOUNCE_LIMIT) {
			break;
		}
		// �V�[���ƃ��C�̌�������
		IntersectionResult result = m_intersector.Intersect(ray, scene, 0);

		// �V�F�[�f�B���O
		if (result.GetType() == INTERSECTION_TYPE::HIT) {
			// �}�e���A��
			Material material = scene.GetMesh(result.GetObjectID())->GetMaterial();

			// �����ɓ��������Ȃ��^�v�Z�I��
			if (scene.GetMesh(result.GetObjectID())->GetTriangleMask() == TRIANGLE_MASK::LIGHT) {
				if (bounce == 0 || isReflected) {
					radiance += material.GetEmittedColor();
					isReflected = false;
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
			float russianRouletteProb = 1.f - material.GetMetallic();

			if (bounce > MINIMUM_BOUNCE) {
				russianRouletteProb *= powf(EPSILON, bounce - MINIMUM_BOUNCE);
			}

			// �g�U����
			if (random < russianRouletteProb) {
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
				alpha *= diffuseBRDF * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / russianRouletteProb);

			}
			// ���ʔ���
			else if (random >= russianRouletteProb && random < russianRouletteProb + material.GetMetallic()) {
				// ����
				if (material.isReflective()) {
					isReflected = true;
					Vector3 reflectDir = Reflect(viewDir, surfaceNormal);
					ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
					alpha *= 1.f / (1.f - russianRouletteProb);
				}
				// ����
				if (material.isRefractive()) {
					isReflected = true;
					// ���˃x�N�g��
					Vector3 reflectDir = Reflect(viewDir, surfaceNormal);

					// ���C���I�u�W�F�N�g�������̂��A�o��̂� ����Ftrue �o��Ffalse
					bool into = Dot(viewDir, surfaceNormal) > 0.f;

					const float nc = 1.f; // �^��̋��ܗ�
					const float ng = material.GetIOR(); // �K���X�̋��ܗ�
					const float nng = into ? nc / ng : ng / nc;

					// ���܃x�N�g��
					Vector3 refractDir = Refract(viewDir, surfaceNormal, nng, into);

					// �S���˂̏ꍇ�͔��ː����݂̂�ǐՂ���
					if (refractDir == Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX)) {
						isReflected = true;
						ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
					}

					Ray refractionRay = Ray(result.GetPosition() + surfaceNormal * EPSILON * (into ? -1.f : 1.f), refractDir);

					// Schlick��Frenel�ߎ���
					const float cos = into ? Dot(surfaceNormal, viewDir) : Dot(surfaceNormal * (into ? -1.f : 1.f), refractDir);

					// ���������S���ʔ��ˁE���S���܂�BSDF�ɊY������
					const float fr = SchlickFresnel(nc, ng, cos);	// ���ˌ��̊���
					// ���܃��C�̉^�ԕ��ˋP�x�͋��ܗ��̈قȂ镨�̂��ړ�����Ƃ��A���ܗ��̔�̓��ω�����
					const float ft = into ? powf(ng / nc, 2.f) : powf(nc / ng, 2.f);

					// ���C���w���I�ɑ�����̂�h�����߁A
					// ���ȏ�̃o�E���X�ȍ~�̓��V�A�����[���b�g�Ŕ��˂����܂ǂ��炩�݂̂�ǐՂ���
					const float prob = fr;

					// ����
					if (randamGenerator(mt) < prob) {
						// Li(x,��) * BRDF / prob
						ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
						alpha *= fr / prob / (1.f - russianRouletteProb);
					}
					// ����
					else {
						// Li(x,��) * BRDF / prob
						ray = Ray(result.GetPosition() + surfaceNormal * EPSILON * (into ? -1.f : 1.f), refractDir);
						alpha *= (1.f - fr) * ft / (1.f - prob) / (1.f - russianRouletteProb);
					}
				}
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
	bool isReflected = false;

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
			if (scene.GetMesh(result.GetObjectID())->GetTriangleMask() == TRIANGLE_MASK::LIGHT) {
				if (bounce == 0 || isReflected) {
					radiance += material.GetEmittedColor();
					isReflected = false;
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
			float russianRouletteProb = 1.f - material.GetMetallic();

			if (bounce > MINIMUM_BOUNCE) {
				russianRouletteProb *= powf(EPSILON, bounce - MINIMUM_BOUNCE);
			}

			// �g�U����
			if (random < russianRouletteProb) {
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
					radiance += alpha * (lightInfo.color * diffuseBRDF * G) / lightInfo.pdf;
				}

				// �V�������C�𐶐�
				ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, incidentDirection);

				// ����������1�_���T���v�����O����
				// �S���̊p��4�΂Ŕ����ɂ��Ă̂ݍl����̂Ŋm�����x�֐�pdf�� 1 / 2��
				// ������BRDF�ɏ]���`�ɂ���Ε��U��ጸ�ł���
				float pdf = 1.f / (2.f * PI);

				// Fr(��,��',x) * cos�� / pdf * (1 / Prr);
				alpha *= diffuseBRDF * Saturate(Dot(surfaceNormal, incidentDirection)) / pdf * (1.f / russianRouletteProb);

				isReflected = false;

			}
			// ���ʔ���
			else if (random >= russianRouletteProb && random < russianRouletteProb + material.GetMetallic()) {
				// ����
				if (material.isReflective()) {
					isReflected = true;
					Vector3 reflectDir = Reflect(viewDir, surfaceNormal);
					ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
					alpha *= 1.f / (1.f - russianRouletteProb);
				}
				// ����
				if (material.isRefractive()) {
					isReflected = true;
					// ���˃x�N�g��
					Vector3 reflectDir = Reflect(viewDir, surfaceNormal);

					// ���C���I�u�W�F�N�g�������̂��A�o��̂� ����Ftrue �o��Ffalse
					bool into = Dot(viewDir, surfaceNormal) > 0.f;

					const float nc = 1.f; // �^��̋��ܗ�
					const float ng = material.GetIOR(); // �K���X�̋��ܗ�
					const float nng = into ? nc / ng : ng / nc;

					// ���܃x�N�g��
					Vector3 refractDir = Refract(viewDir, surfaceNormal, nng, into);

					// �S���˂̏ꍇ�͔��ː����݂̂�ǐՂ���
					if (refractDir == Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX)) {
						isReflected = true;
						ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
					}

					Ray refractionRay = Ray(result.GetPosition() + surfaceNormal * EPSILON * (into ? -1.f : 1.f), refractDir);

					// Schlick��Frenel�ߎ���
					const float cos = into ? Dot(surfaceNormal, viewDir) : Dot(surfaceNormal * (into ? -1.f : 1.f), refractDir);

					// ���������S���ʔ��ˁE���S���܂�BSDF�ɊY������
					const float fr = SchlickFresnel(nc, ng, cos);	// ���ˌ��̊���
					// ���܃��C�̉^�ԕ��ˋP�x�͋��ܗ��̈قȂ镨�̂��ړ�����Ƃ��A���ܗ��̔�̓��ω�����
					const float ft = into ? powf(ng / nc, 2.f) : powf(nc / ng, 2.f);

					// ���C���w���I�ɑ�����̂�h�����߁A
					// ���ȏ�̃o�E���X�ȍ~�̓��V�A�����[���b�g�Ŕ��˂����܂ǂ��炩�݂̂�ǐՂ���
					const float prob = fr;

					// ����
					if (randamGenerator(mt) < prob) {
						// Li(x,��) * BRDF / prob
						ray = Ray(result.GetPosition() + surfaceNormal * EPSILON, reflectDir);
						alpha *= fr / prob / (1.f - russianRouletteProb);
					}
					// ����
					else {
						// Li(x,��) * BRDF / prob
						ray = Ray(result.GetPosition() + surfaceNormal * EPSILON * (into ? -1.f : 1.f), refractDir);
						alpha *= (1.f - fr) * ft / (1.f - prob) / (1.f - russianRouletteProb);
					}
				}
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

const float PathTracer::Renderer::SchlickFresnel(float etai, float etat, float cos)
{
	float f0 = powf((etai - etat), 2.f) / powf((etai + etat), 2.f);

	return f0 + (1.f - f0) * powf((1.f - cos), 5.f);
}

const bool PathTracer::Renderer::RussianRoulette(const Material& material)
{
	// ��l�����𐶐�
	std::random_device device;
	std::mt19937_64 mt(device());
	std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
	float random = randamGenerator(mt);
	// ���ˊm���̍ő�l��0.5�Ƃ���
	float russianRouletteProb = Min(1.f - material.GetMetallic(), 0.5f);
	if (random < russianRouletteProb) {
		return true;
	}
	else {
		return false;
	}
}
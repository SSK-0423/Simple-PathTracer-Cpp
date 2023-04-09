#include "AreaLight.hpp"
#include <random>

namespace PathTracer {
	const LightInfo AreaLight::Sample(const Vector3& position)
	{
		// 一様乱数生成
		std::random_device device;
		std::mt19937_64 mt(device());
		std::uniform_real_distribution<> randamGenerator(0.f, 1.f);
		float u1 = randamGenerator(mt);
		float u2 = randamGenerator(mt);

		// 面上のサンプル位置算出
		// 面の中心からの差分を示す値を算出するために，0.0～1.0を-1.0～1.0にマップ
		// さらに、光源のサイズに合わせてサンプルする
		float u = u1 * 2.f - 1.f;
		float v = u2 * 2.f - 1.f;
		// 面にマッピング
		auto right = m_transform.GetRight();
		auto up = m_transform.GetUp();
		Vector3 worldSamplePointPos = m_transform.GetPosition()
			+ right * u * m_transform.GetScale().x / 2.f
			+ up * v * m_transform.GetScale().y / 2.f;

		Vector3 toSamplePoint = worldSamplePointPos - position;

		LightInfo info;
		info.color = m_color;
		info.direction = Normalize(toSamplePoint);
		info.distance = (toSamplePoint).Length();
		info.pdf = 1.f / (m_transform.GetScale().x * m_transform.GetScale().y);
		info.normal = m_transform.GetForward();
		info.type = LIGHT_TYPE::AREA;

		return info;
	}
}
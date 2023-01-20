#include "TestScene.hpp"
#include "../Utility/Vector.hpp"
#include "../Graphics/Sphere.hpp"
#include "../Graphics/TestTriangle.hpp"

PathTracer::TestScene::TestScene()
{
}

PathTracer::TestScene::~TestScene()
{
}

void PathTracer::TestScene::Init()
{
	//m_meshes.push_back(std::make_shared<Sphere>(1.f, Vector3(0.f, 0.f, 5.f), 0, Material()));
	Transform transform = Transform(
		Vector3(1, 0, 0),
		Vector3(0, 0, 90),
		Vector3(1, 1, 1));

	m_meshes.push_back(std::make_shared<Triangle>(0, Material(), transform));
}

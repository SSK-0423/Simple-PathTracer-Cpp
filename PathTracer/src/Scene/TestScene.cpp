#include "TestScene.hpp"
#include "../Utility/Vector.hpp"
#include "../Graphics/Sphere.hpp"
#include "../Graphics/Mesh.hpp"

PathTracer::TestScene::TestScene()
{
}

PathTracer::TestScene::~TestScene()
{
}

void PathTracer::TestScene::Init()
{
	m_spheres.push_back(std::make_shared<Sphere>(1.f, Vector3(0.f, 0.f, 3.f), 0));
}

#include "CornellBox.hpp"
#include "../Utility/Vector.hpp"

#include "../Graphics/Mesh.hpp"
#include "../Graphics/Sphere.hpp"
#include "../Graphics/Plane.hpp"
#include "../Graphics/Cube.hpp"

#include "../Graphics/DirectionalLight.hpp"
#include "../Graphics/PointLight.hpp"

PathTracer::CornellBox::CornellBox()
{
}

PathTracer::CornellBox::~CornellBox()
{
}

void PathTracer::CornellBox::Init()
{
	Material redMaterial = Material(Vector3(0.75, 0.25, 0.25), 0.f, 1.f);
	Material blueMaterial = Material(Vector3(0.25, 0.25, 0.75), 0.f, 1.f);
	Material whiteMaterial = Material(Vector3(0.75, 0.75, 0.75), 0.f, 1.f);
	Material greenMaterial = Material(Vector3(0.25, 0.75, 0.25), 0.f, 1.f);
	Material grayMaterial = Material(Vector3(0.725f, 0.71f, 0.68f), 0.f, 1.f);
	Material emittedMaterial = Material(Vector3(1.f, 1.f, 1.f), 0.f, 1.f, Vector3(5.f, 5.f, 5.f));
	Material reflectiveMaterial = Material(Vector3(1.f, 1.f, 1.f), 1.f, 0.f, Vector3(0.f, 0.f, 0.f), true);

	// 光源
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(0.f, 0.99f, 0.f), Vector3(90.f, 0.f, 0.f), Vector3(0.5f, 0.5f, 0.5f)), emittedMaterial, 0));

	// 白床
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(0.f, -1.f, 0.f), Vector3(-90.f, 0.f, 0.f), Vector3(2.f, 2.f, 2.f)), whiteMaterial, 1));
	// 白天井
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(0.f, 1.f, 0.f), Vector3(90.f, 0.f, 0.f), Vector3(2.f, 2.f, 2.f)), whiteMaterial, 2));
	// 白壁
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(0.f, 0.f, 1.f), Vector3(0.f, 180.f, 0.f), Vector3(2.f, 2.f, 2.f)), whiteMaterial, 3));
	// 赤壁
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 90.f, 0.f), Vector3(2.f, 2.f, 2.f)), redMaterial, 4));
	// 緑壁
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(1.f, 0.f, 0.f), Vector3(0.f, -90.f, 0.f), Vector3(2.f, 2.f, 2.f)), greenMaterial, 5));

	// キューブ
	m_meshes.push_back(std::make_shared<Cube>(
		Transform(Vector3(0.3275f, -0.7f, -0.3725f), Vector3(0.f, RadToDeg(0.3f), 0.f), Vector3(0.6f, 0.6f, 0.6f)), grayMaterial, 8));
	m_meshes.push_back(std::make_shared<Cube>(
		Transform(Vector3(-0.335f, -0.4f, 0.29f), Vector3(0.f, RadToDeg(-0.3f), 0.f), Vector3(0.6f, 1.2f, 0.6f)), reflectiveMaterial, 9));

	// 球体
	//m_meshes.push_back(std::make_shared<Sphere>(0.35f, 16, 16, Transform(Vector3(-0.45f, -0.65f, 0.29f)), whiteMaterial, 6));
	//m_meshes.push_back(std::make_shared<Sphere>(0.35f, 16, 16, Transform(Vector3(0.5f, -0.65f, -0.43f)), whiteMaterial, 7));
}

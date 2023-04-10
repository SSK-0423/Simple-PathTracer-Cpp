#include "CornellBox.hpp"
#include "../Utility/Vector.hpp"

#include "../Graphics/Mesh.hpp"
#include "../Graphics/Sphere.hpp"
#include "../Graphics/Plane.hpp"
#include "../Graphics/Cube.hpp"
#include "../Graphics/OBJModel.hpp"

#include "../Graphics/DirectionalLight.hpp"
#include "../Graphics/PointLight.hpp"
#include "../Graphics/AreaLight.hpp"

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
	Material skyBlueMaterial = Material(Vector3(0.f, 0.5f, 0.9f), 0.f, 1.f);
	Material emittedMaterial = Material(Vector3(1.f, 1.f, 1.f), 0.f, 1.f, Vector3(4.f, 1.5f, 0.25f));
	//Material emittedMaterial = Material(Vector3(1.f, 1.f, 1.f), 0.f, 1.f, Vector3(5.f, 5.f, 5.f));
	Material reflectiveMaterial = Material(Vector3(1.f, 1.f, 1.f), 1.f, 0.f, Vector3(0.f, 0.f, 0.f), true);

	// 光源
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(0.f, 0.99f, 0.f), Vector3(90.f, 0.f, 0.f), Vector3(0.75f, 0.75f, 0.75f)), emittedMaterial, 0, TRIANGLE_MASK::LIGHT));

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

	// 鏡
	//m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(-0.99f, 0.f, 0.f), Vector3(0.f, 90.f, 0.f), Vector3(0.5f, 0.5f, 0.5f)), reflectiveMaterial, 12));
	// キューブ
	//m_meshes.push_back(std::make_shared<Cube>(
	//	Transform(Vector3(0.3275f, -0.7f, -0.3725f), Vector3(0.f, RadToDeg(0.3f), 0.f), Vector3(0.6f, 0.6f, 0.6f)), grayMaterial, 6));
	//m_meshes.push_back(std::make_shared<Cube>(
	//	Transform(Vector3(-0.335f, -0.4f, 0.29f), Vector3(0.f, RadToDeg(-0.3f), 0.f), Vector3(0.6f, 1.2f, 0.6f)), grayMaterial, 7));

	m_meshes.push_back(std::make_shared<Cube>(
		Transform(Vector3(-0.335f, -0.7f, -0.1f), Vector3(0.f, RadToDeg(0.3f), 0.f), Vector3(0.6f, 0.6f, 0.6f)), grayMaterial, 7));
	//m_meshes.push_back(std::make_shared<Cube>(
	//	Transform(Vector3(-0.335f, 0.f, 0.5f), Vector3(0.f, 0.f, 0.f), Vector3(0.3f, 0.3f, 0.3f)), skyBlueMaterial, 13));

	m_meshes.push_back(std::make_shared<OBJModel>(
		"low_poly_bunny.obj", Transform(Vector3(-0.1f, -0.45f, 0.f), Vector3(0.f, 180.f - RadToDeg(0.15f), 0.f), Vector3(0.008, 0.008, 0.008)), blueMaterial, 10));

	//m_meshes.push_back(std::make_shared<OBJModel>(
	//	"bunny.obj", Transform(Vector3(0.f, -0.5f, 0.f), Vector3(0.f, 0.f, 0.f), Vector3(0.5f, 0.5f, 0.5f)), blueMaterial, 10));

	//m_meshes.push_back(std::make_shared<OBJModel>(
	//	"bunny2.obj", Transform(Vector3(0.f, -0.5f, 0.f), Vector3(0.f, 0.f, 0.f), Vector3(0.125f, 0.125f, 0.125f)), whiteMaterial, 10));

	// 面光源
	m_lights.push_back(std::make_shared<AreaLight>(emittedMaterial.GetEmittedColor(), Transform(Vector3(0.f, 0.98f, 0.f), Vector3(90.f, 0.f, 0.f), Vector3(0.75f, 0.75f, 0.75f))));

	// 球体
	//m_meshes.push_back(std::make_shared<Sphere>(0.35f, 16, 16, Transform(Vector3(-0.45f, -0.65f, 0.29f)), blueMaterial, 6));
	//m_meshes.push_back(std::make_shared<Sphere>(0.35f, 16, 16, Transform(Vector3(0.5f, -0.65f, -0.43f)), reflectiveMaterial, 7));
	m_meshes.push_back(std::make_shared<Sphere>(0.35f, 16, 16, Transform(Vector3(0.5f, -0.65f, -0.43f)), reflectiveMaterial, 11));

	m_bvh.Construct(*this);
}

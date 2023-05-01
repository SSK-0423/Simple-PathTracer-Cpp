#include "CornellBoxInWater.hpp"
#include "../Utility/Vector.hpp"

#include "../Graphics/Mesh.hpp"
#include "../Graphics/Sphere.hpp"
#include "../Graphics/Plane.hpp"
#include "../Graphics/Cube.hpp"
#include "../Graphics/OBJModel.hpp"

#include "../Graphics/DirectionalLight.hpp"
#include "../Graphics/PointLight.hpp"
#include "../Graphics/AreaLight.hpp"

PathTracer::CornellBoxInWater::CornellBoxInWater()
{
}

PathTracer::CornellBoxInWater::~CornellBoxInWater()
{
}

void PathTracer::CornellBoxInWater::Init()
{
	Material redMaterial = Material(Vector3(0.75, 0.25, 0.25), 0.f, 1.f);
	Material blueMaterial = Material(Vector3(0.25, 0.25, 0.75), 0.f, 1.f);
	Material whiteMaterial = Material(Vector3(0.75, 0.75, 0.75), 0.f, 1.f);
	Material greenMaterial = Material(Vector3(0.25, 0.75, 0.25), 0.f, 1.f);
	Material grayMaterial = Material(Vector3(0.725f, 0.71f, 0.68f), 0.f, 1.f);
	Material skyBlueMaterial = Material(Vector3(0.f, 0.5f, 0.9f), 0.f, 1.f);
	Material emittedMaterial = Material(Vector3(1.f, 1.f, 1.f), 0.f, 1.f, Vector3(5.f, 5.f, 5.f));
	Material reflectiveMaterial = Material(Vector3(1.f, 1.f, 1.f), 1.f, 0.f, Vector3(0.f, 0.f, 0.f), true);
	Material refractiveMaterial = Material(Vector3(1.f, 1.f, 1.f), 1.f, 0.f, Vector3(0.f, 0.f, 0.f), false, true, 1.51673f);

	Material waterMaterial = Material(Vector3(0.f, 0.8f, 0.9f), 1.f, 0.f, Vector3(0.f, 0.f, 0.f), false, true, 1.333f);

	// モデルが小さすぎると交差判定がうまくいかないので大きくする
	float scale = 4.f;

	// 光源
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(0.f, 0.99f * scale, 0.f), Vector3(90.f, 0.f, 0.f), Vector3(0.75f * scale, 0.75f * scale, 0.75f * scale)), emittedMaterial, 0, TRIANGLE_MASK::LIGHT));

	// 白床
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(0.f, -1.f * scale, 0.f), Vector3(-90.f, 0.f, 0.f), Vector3(2.f * scale, 2.f * scale, 2.f * scale)), whiteMaterial, 1));
	// 白天井
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(0.f, 1.f * scale, 0.f), Vector3(90.f, 0.f, 0.f), Vector3(2.f * scale, 2.f * scale, 2.f * scale)), whiteMaterial, 2));
	// 白壁
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(0.f, 0.f, 1.f * scale), Vector3(0.f, 180.f, 0.f), Vector3(2.f * scale, 2.f * scale, 2.f * scale)), whiteMaterial, 3));
	// 赤壁
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(-1.f * scale, 0.f, 0.f), Vector3(0.f, 90.f, 0.f), Vector3(2.f * scale, 2.f * scale, 2.f * scale)), redMaterial, 4));
	// 緑壁
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(1.f * scale, 0.f, 0.f), Vector3(0.f, -90.f, 0.f), Vector3(2.f * scale, 2.f * scale, 2.f * scale)), blueMaterial, 5));

	// キューブ
	m_meshes.push_back(std::make_shared<Cube>(
		Transform(Vector3(-0.335f * scale, -0.4f * scale, 0.29f * scale), Vector3(0.f, RadToDeg(-0.3f), 0.f), Vector3(0.6f * scale, 1.2f * scale, 0.6f * scale)), reflectiveMaterial, 7));
	m_meshes.push_back(std::make_shared<Cube>(
		Transform(Vector3(0.3275f * scale, -0.7f * scale, -0.3725f * scale), Vector3(0.f, RadToDeg(0.3f), 0.f), Vector3(0.6f * scale, 0.6f * scale, 0.6f * scale)), grayMaterial, 6));

	// 面光源
	m_lights.push_back(std::make_shared<AreaLight>(emittedMaterial.GetEmittedColor(), Transform(Vector3(0.f, 0.98f * scale, 0.f), Vector3(90.f, 0.f, 0.f), Vector3(0.75f * scale, 0.75f * scale, 0.75f * scale))));

	// 球体
	//m_meshes.push_back(std::make_shared<OBJModel>("sphere.obj", Transform(Vector3(-0.45f * scale, -0.65f * scale, 0.29f * scale), Vector3(), Vector3(0.35 * scale, 0.35 * scale, 0.35 * scale)), reflectiveMaterial, 6));
	//m_meshes.push_back(std::make_shared<OBJModel>("sphere.obj", Transform(Vector3(0.5f * scale, -0.65f * scale, -0.43f * scale), Vector3(), Vector3(0.35 * scale, 0.35 * scale, 0.35 * scale)), refractiveMaterial, 7));

	// 水面
	m_meshes.push_back(std::make_shared<OBJModel>("water2.obj", Transform(Vector3(0.f, -1.f * scale, 0.f), Vector3(0.f, 180.f, 0.f), Vector3(1 * scale, 1 * scale, 1 * scale)), refractiveMaterial, 8));

	m_bvh.Construct(*this);
}
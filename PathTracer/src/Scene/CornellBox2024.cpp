#include "CornellBox2024.hpp"
#include "../Utility/Vector.hpp"

#include "../Graphics/Mesh.hpp"
#include "../Graphics/Sphere.hpp"
#include "../Graphics/Plane.hpp"
#include "../Graphics/Cube.hpp"
#include "../Graphics/OBJModel.hpp"

#include "../Graphics/DirectionalLight.hpp"
#include "../Graphics/PointLight.hpp"
#include "../Graphics/AreaLight.hpp"

PathTracer::CornellBox2024::CornellBox2024()
{
}

PathTracer::CornellBox2024::~CornellBox2024()
{
}

void PathTracer::CornellBox2024::Init()
{
	Material redMaterial = Material(Vector3(0.75, 0.25, 0.25), 0.f, 1.f);
	Material blueMaterial = Material(Vector3(0.0, 0.22, 0.8), 0.f, 1.f);
	Material whiteMaterial = Material(Vector3(0.75, 0.75, 0.75), 0.f, 1.f);
	Material greenMaterial = Material(Vector3(0.25, 0.75, 0.25), 0.f, 1.f);
	Material grayMaterial = Material(Vector3(0.725f, 0.71f, 0.68f), 0.f, 1.f);
	Material skyBlueMaterial = Material(Vector3(0.f, 0.5f, 0.9f), 0.f, 1.f);
	Material yellowMaterial = Material(Vector3(0.8f, 0.58f, 0.f), 0.f, 1.f);

	Material emittedMaterial = Material(Vector3(1.f, 1.f, 1.f), 0.f, 1.f, Vector3(5.f, 5.f, 5.f));

	Material metallicMaterial = Material(Vector3(1.f, 1.f, 1.f), 0.25f, 0.f, Vector3(0.f, 0.f, 0.f), true);
	Material reflectionMaterial = Material(Vector3(1.f, 1.f, 1.f), 1.f, 0.f, Vector3(0.f, 0.f, 0.f), true);
	Material glassMaterial = Material(Vector3(1.f, 1.f, 1.f), 1.f, 0.f, Vector3(0.f, 0.f, 0.f), false, true, 1.5f);

	Material goldMaterial = Material(Vector3(0.85f, 0.7f, 0.f), 0.f, 0.f, Vector3(0.8f * 0.5f, 0.7f * 5.f, 0.f));
	Material textMaterial = Material(Vector3(0.94f, 0.38f, 0.38f), 0.f, 0.f);

	float scale = 15.f;

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
	m_meshes.push_back(std::make_shared<Plane>(Transform(Vector3(1.f * scale, 0.f, 0.f), Vector3(0.f, -90.f, 0.f), Vector3(2.f * scale, 2.f * scale, 2.f * scale)), greenMaterial, 5));

	// ドラゴン
	m_meshes.push_back(
		std::make_shared<OBJModel>("dragon.obj",
			Transform(
				Vector3(0.f, -0.5f * scale, 0.f),
				Vector3(0.f, 0.f, 0.f),
				Vector3(1.75f * scale, 1.75f * scale, 1.75f * scale)),
			goldMaterial, 6));

	// 2024テキスト
	m_meshes.push_back(
		std::make_shared<OBJModel>("2024.obj",
			Transform(
				Vector3(0, -0.7f * scale, -0.5f * scale),
				Vector3(0.f, 0.f, 0.f),
				Vector3(0.75f * scale, 0.75f * scale, 1.f * scale)),
			textMaterial, 7));

	// 面光源
	m_lights.push_back(std::make_shared<AreaLight>(emittedMaterial.GetEmittedColor(), Transform(Vector3(0.f, 0.98f * scale, 0.f), Vector3(90.f, 0.f, 0.f), Vector3(0.75f * scale, 0.75f * scale, 0.75f * scale))));

	m_bvh.Construct(*this);
}

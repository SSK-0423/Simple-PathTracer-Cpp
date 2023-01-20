#include "CornellBox.hpp"
#include "../Utility/Vector.hpp"

#include "../Graphics/Mesh.hpp"
#include "../Graphics/Sphere.hpp"
#include "../Graphics/Plane.hpp"

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

	// ”’°
	m_meshes.push_back(std::make_shared<Plane>(Vector3(0, -1, 0), Vector3(0, 1, 0), 0, whiteMaterial));
	// ”’“Vˆä
	m_meshes.push_back(std::make_shared<Plane>(Vector3(0, 1, 0), Vector3(0, -1, 0), 1, whiteMaterial));
	// Ô•Ç
	m_meshes.push_back(std::make_shared<Plane>(Vector3(1, 0, 0), Vector3(-1, 0, 0), 2, redMaterial));
	// Â•Ç
	m_meshes.push_back(std::make_shared<Plane>(Vector3(-1, 0, 0), Vector3(1, 0, 0), 3, greenMaterial));
	// ”’•Ç
	m_meshes.push_back(std::make_shared<Plane>(Vector3(0, 0, 1), Vector3(0, 0, -1), 4, whiteMaterial));

	// ‹…‘Ì
	m_meshes.push_back(std::make_shared<Sphere>(0.35f, Vector3(-0.45f, -0.65f, 0.29f), 5, reflectiveMaterial));
	m_meshes.push_back(std::make_shared<Sphere>(0.35f, Vector3(0.5f, -0.65f, -0.43f), 6, whiteMaterial));

	// ŒõŒ¹‚Æ‚È‚é‹…‘Ì
	//m_meshes.push_back(std::make_shared<Sphere>(1.f, Vector3(0.f, 2 * 1.f - 0.1f, 0.f), 7, emittedMaterial));
	m_meshes.push_back(std::make_shared<Sphere>(10.f, Vector3(0.f, 2 * 10.f - 9.00625f, 0.f), 7, emittedMaterial));
	//float lightRadius = 1e5;
	//m_meshes.push_back(std::make_shared<Sphere>(lightRadius, Vector3(0.f, 2 * lightRadius - 1e5 + 1.0039062500056, 0.5f), 7, emittedMaterial));

	// ŒõŒ¹
	//m_lights.push_back(std::make_shared<DirectionalLight>(Vector3(1, 1, -1), Vector3(1, 1, 1)));
	m_lights.push_back(std::make_shared<PointLight>(Vector3(0, 0.99, 0.0), Vector3(5, 5, 5)));
}

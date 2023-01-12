#include "CornellBox.hpp"
#include "../Utility/Vector.hpp"
#include "../Graphics/Sphere.hpp"
#include "../Graphics/Plane.hpp"
#include "../Graphics/Mesh.hpp"

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
	Material grayMaterial = Material(Vector3(0.725f, 0.71f, 0.68f));

	// ”’°
	m_meshes.push_back(std::make_shared<Plane>(Vector3(0, -1, 0), Vector3(0, 1, 0), 0, whiteMaterial));
	// ”’“Vˆä
	m_meshes.push_back(std::make_shared<Plane>(Vector3(0, 1, 0), Vector3(0, -1, 0), 1, whiteMaterial));
	// Ô•Ç
	m_meshes.push_back(std::make_shared<Plane>(Vector3(1, 0, 0), Vector3(-1, 0, 0), 2, redMaterial));
	// Â•Ç
	m_meshes.push_back(std::make_shared<Plane>(Vector3(-1, 0, 0), Vector3(1, 0, 0), 3, greenMaterial));
	// ”’•Ç
	m_meshes.push_back(std::make_shared<Plane>(Vector3(0, 0, 2), Vector3(0, 0, -1), 4, whiteMaterial));

	// ‹…‘Ì
	m_meshes.push_back(std::make_shared<Sphere>(0.35f, Vector3(-0.4f, -0.65f, 1.5f), 5, blueMaterial));
	m_meshes.push_back(std::make_shared<Sphere>(0.35f, Vector3(0.5f, -0.65f, 1.25f), 6, grayMaterial));
}

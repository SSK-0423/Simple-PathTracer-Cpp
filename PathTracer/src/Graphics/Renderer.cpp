#include "Renderer.hpp"

PathTracer::Renderer::Renderer()
{
}

PathTracer::Renderer::~Renderer()
{
}

void PathTracer::Renderer::Init(unsigned int width, unsigned int height)
{
	m_width = width;
	m_height = height;
	m_renderTarget.Create(width, height);

	m_camera = Camera(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector3(1, 0, 0), Vector3(0, 1, 0));
}

void PathTracer::Renderer::Render()
{

	// パストレーシング
	for (size_t y = 0; y < m_height; y++) {
		for (size_t x = 0; x < m_width; x++) {
			// カメラレイを生成
			Vector3 cameraDir = Saturate(m_camera.GetCameraRay(x, y, m_width, m_height).GetDirection());
			m_renderTarget.Write(x, y, cameraDir.r, cameraDir.g, cameraDir.b);
		}
	}

	// パストレーシング結果を出力
	m_renderTarget.OutputImage("test.ppm");
}

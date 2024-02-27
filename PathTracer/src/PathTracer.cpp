#include "PathTracer.hpp"

PathTracer::PathTracer::PathTracer()
{
}

PathTracer::PathTracer::~PathTracer()
{
}

PathTracer::RESULT PathTracer::PathTracer::Init(const unsigned int& width, const unsigned int& height, const unsigned int& sampleCount)
{
	m_renderer.Init(width, height, sampleCount);

	return RESULT::SUCCESS;
}

void PathTracer::PathTracer::Render(const Scene& scene, const std::string& outputPath)
{
	m_renderer.Render(scene, outputPath);
}

void PathTracer::PathTracer::Final()
{
}

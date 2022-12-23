#include "PathTracer.hpp"

PathTracer::PathTracer::PathTracer()
{
}

PathTracer::PathTracer::~PathTracer()
{
}

PathTracer::RESULT PathTracer::PathTracer::Init(unsigned int width, unsigned int height)
{
	m_renderer.Init(width, height);

	return RESULT::SUCCESS;
}

void PathTracer::PathTracer::Render()
{
	m_renderer.Render();
}

void PathTracer::PathTracer::Final()
{
}

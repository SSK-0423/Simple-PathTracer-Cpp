#pragma once
#include "PixelBuffer.hpp"
#include "../Scene/Camera.hpp"
#include "../Scene/Scene.hpp"

namespace PathTracer {
	class Renderer {
	public:
		Renderer();
		~Renderer();

		void Init(unsigned int width, unsigned int height);

		void Render();
	private:
		unsigned int m_width;
		unsigned int m_height;
		PixelBuffer m_renderTarget;

		Camera m_camera;
	};
}
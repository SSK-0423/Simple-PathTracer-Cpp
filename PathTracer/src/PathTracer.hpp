#pragma once
#include <memory>

#include "Graphics/Renderer.hpp"
#include "Graphics/PixelBuffer.hpp"

#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"

#include "Utility/Utility.hpp"
#include "Utility/Vector.hpp"
#include "Utility/Matrix.hpp"

namespace PathTracer {
	class PathTracer {
	public:
		PathTracer();
		~PathTracer();
		
		RESULT Init(unsigned int width, unsigned int height);
		void Render();
		void Final();

	private:
		Renderer m_renderer;
	};
}
#pragma once
#include <memory>

#include "Graphics/Renderer.hpp"
#include "Graphics/PixelBuffer.hpp"

#include "Utility/Utility.hpp"
#include "Utility/Vector.hpp"
#include "Utility/Matrix.hpp"

namespace PathTracer {
	class PathTracer {
	public:
		PathTracer();
		~PathTracer();

		RESULT Init(unsigned int width, unsigned int height);
		void Render(const Scene& scene);
		void Final();

	private:
		Renderer m_renderer;
	};
}
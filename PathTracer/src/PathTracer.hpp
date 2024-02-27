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

		RESULT Init(const unsigned int& width, const unsigned int& height, const unsigned int& sampleCount);
		void Render(const Scene& scene, const std::string& outputPath);
		void Final();

	private:
		Renderer m_renderer;
	};
}
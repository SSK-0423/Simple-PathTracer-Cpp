#pragma once
#include "../Utility/Vector.hpp"
#include <memory>
#include <string>

namespace PathTracer {
	class PixelBuffer {
	public:
		PixelBuffer();
		~PixelBuffer();

		void Create(unsigned int width, unsigned int height);
		void Write(unsigned int x, unsigned int y, float r, float g, float b);
		void OutputImage(const std::string& filePath);

		unsigned int GetWidth() { return m_width; }
		unsigned int GetHeight() { return m_height; }

	private:
		unsigned int m_width;
		unsigned int m_height;
		std::unique_ptr<Vector3[]> m_pixels;
	};
}
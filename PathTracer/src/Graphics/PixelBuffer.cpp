#include <fstream>
#include <string>
#include <iostream>

#include "PixelBuffer.hpp"

constexpr unsigned int MAX_RADIANCE = 255;

PathTracer::PixelBuffer::PixelBuffer() : m_width(0), m_height(0)
{
}

PathTracer::PixelBuffer::~PixelBuffer()
{
}

void PathTracer::PixelBuffer::Create(unsigned int width, unsigned int height)
{
	m_width = width;
	m_height = height;
	m_pixels = std::make_unique<Vector3[]>(width * height);
}

void PathTracer::PixelBuffer::Write(unsigned int x, unsigned int y, float r, float g, float b)
{
	m_pixels[x + y * m_width].r = r;
	m_pixels[x + y * m_width].g = g;
	m_pixels[x + y * m_width].b = b;
}

void PathTracer::PixelBuffer::OutputImage(const std::string& filePath)
{
	// ppm画像として出力する
	std::ofstream ppmFile(filePath);
	if (!ppmFile) {
		std::cout << "ファイルオープンに失敗しました。" << std::endl;
	}
	else {
		std::cout << "ファイルをオープンしました。" << std::endl;
	}

	ppmFile << "P3" << std::endl;	// フォーマット指定
	ppmFile << m_width << " " << m_height << std::endl;	// 幅と高さを指定
	ppmFile << MAX_RADIANCE << std::endl;	// 最大輝度指定

	// ピクセル毎の輝度を書き込む
	for (size_t h = 0; h < m_height; h++) {
		for (size_t w = 0; w < m_width; w++) {
			Vector3 radiance = m_pixels[h * m_width + w] * MAX_RADIANCE;
			// 浮動小数のまま書き込むと小数点「.」が区切り文字として認識されるため
			// レンダリング結果が正しく表示されなくなる
			ppmFile << static_cast<unsigned int>(radiance.r) << " " << static_cast<unsigned int>(radiance.g) << " " << static_cast<unsigned int>(radiance.b) << " ";
		}
	}

	ppmFile.close();
}
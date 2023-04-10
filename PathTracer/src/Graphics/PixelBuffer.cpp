#include <fstream>
#include <string>
#include <iostream>

#include "PixelBuffer.hpp"
#include "../Utility/PathTracerMath.hpp"

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

	for (size_t i = 0; i < width * height; i++) {
		m_pixels[i] = Vector3(0.f, 0.f, 0.f);
	}
}

void PathTracer::PixelBuffer::Write(unsigned int x, unsigned int y, float r, float g, float b)
{
	m_pixels[x + y * m_width].x = Saturate(r);
	m_pixels[x + y * m_width].y = Saturate(g);
	m_pixels[x + y * m_width].z = Saturate(b);
}

void PathTracer::PixelBuffer::OutputImage(const std::string& filePath)
{
	// ppm�摜�Ƃ��ďo�͂���
	std::ofstream ppmFile(filePath);
	if (!ppmFile) {
		std::cout << "�t�@�C���I�[�v���Ɏ��s���܂����B" << std::endl;
	}
	else {
		std::cout << "�t�@�C�����I�[�v�����܂����B" << std::endl;
	}

	ppmFile << "P3" << std::endl;	// �t�H�[�}�b�g�w��
	ppmFile << m_width << " " << m_height << std::endl;	// ���ƍ������w��
	ppmFile << MAX_RADIANCE << std::endl;	// �ő�P�x�w��

	// �s�N�Z�����̋P�x����������
	for (size_t h = 0; h < m_height; h++) {
		for (size_t w = 0; w < m_width; w++) {
			Vector3 radiance = m_pixels[h * m_width + w];
			radiance.x = powf(radiance.r(), 1.f / 2.2f);
			radiance.y = powf(radiance.g(), 1.f / 2.2f);
			radiance.z = powf(radiance.b(), 1.f / 2.2f);
			radiance *= MAX_RADIANCE;
			// ���������̂܂܏������ނƏ����_�u.�v����؂蕶���Ƃ��ĔF������邽��
			// �����_�����O���ʂ��������\������Ȃ��Ȃ�
			ppmFile << static_cast<unsigned int>(radiance.r()) << " " << static_cast<unsigned int>(radiance.g()) << " " << static_cast<unsigned int>(radiance.b()) << " ";
		}
	}

	ppmFile.close();
}
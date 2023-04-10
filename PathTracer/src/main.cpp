#include <iostream>
#include "PathTracer.hpp"
#include "Scene/Scene.hpp"
#include "Scene/TestScene.hpp"
#include "Scene/CornellBox.hpp"

int main() {
	PathTracer::PathTracer pathTracer;
	PathTracer::TestScene testScene;
	PathTracer::CornellBox cornellBox;
	testScene.Init();
	cornellBox.Init();


	if (pathTracer.Init(512, 512, 1) == PathTracer::RESULT::FAILED) {
		std::cout << "�p�X�g���[�T�[�̏������Ɏ��s" << std::endl;
	}
	pathTracer.Render(cornellBox);
	pathTracer.Final();

	return 0;
}
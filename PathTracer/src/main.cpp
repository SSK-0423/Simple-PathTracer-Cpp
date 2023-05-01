#include <iostream>
#include "PathTracer.hpp"
#include "Scene/Scene.hpp"
#include "Scene/TestScene.hpp"
#include "Scene/CornellBox.hpp"
#include "Scene/CornellBoxInWater.hpp"

int main() {
	PathTracer::PathTracer pathTracer;
	PathTracer::CornellBox cornellBox;
	PathTracer::CornellBoxInWater cornellBoxInWater;
	//cornellBox.Init();
	cornellBoxInWater.Init();

	if (pathTracer.Init(512, 512, 50) == PathTracer::RESULT::FAILED) {
		std::cout << "パストレーサーの初期化に失敗" << std::endl;
	}
	//pathTracer.Render(cornellBox);
	pathTracer.Render(cornellBoxInWater);
	pathTracer.Final();

	return 0;
}
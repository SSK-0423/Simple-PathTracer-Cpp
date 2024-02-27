#include <iostream>
#include "PathTracer.hpp"
#include "Scene/Scene.hpp"
#include "Scene/TestScene.hpp"
#include "Scene/CornellBox.hpp"
#include "Scene/CornellBoxInWater.hpp"
#include "Scene/CornellBox2024.hpp"

int main() {
	PathTracer::PathTracer pathTracer;
	PathTracer::CornellBox cornellBox;
	PathTracer::CornellBoxInWater cornellBoxInWater;
	PathTracer::CornellBox2024 cornellBox2024;
	//cornellBox.Init();
	//cornellBoxInWater.Init();
	cornellBox2024.Init();

	if (pathTracer.Init(512, 512, 1) == PathTracer::RESULT::FAILED) {
		std::cout << "パストレーサーの初期化に失敗" << std::endl;
	}
	//pathTracer.Render(cornellBox,"../RenderImage/CornellBox.ppm");
	//pathTracer.Render(cornellBoxInWater);
	pathTracer.Render(cornellBox2024, "../RenderImage/CornellBox2024.ppm");

	pathTracer.Final();

	return 0;
	//45
}
#include <iostream>
#include "PathTracer.hpp"

int main() {
	PathTracer::PathTracer pathTracer;
	if (pathTracer.Init(512,512) == PathTracer::RESULT::FAILED) {
		std::cout << "パストレーサーの初期化に失敗" << std::endl;
	}
	pathTracer.Render();
	pathTracer.Final();

	return 0;
}
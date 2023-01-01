#pragma once
#include "Scene.hpp"
namespace PathTracer {
	class TestScene : public Scene {
	public:
		TestScene();
		~TestScene();

		void Init() override;
	};
}
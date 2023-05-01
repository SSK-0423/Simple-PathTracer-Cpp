#pragma once
#include "Scene.hpp"

namespace PathTracer {
	class CornellBoxInWater : public Scene {
	public:
		CornellBoxInWater();
		~CornellBoxInWater();

		void Init() override;
	};
}
#pragma once
#include "Scene.hpp"

namespace PathTracer {
	class CornellBox2024 : public Scene {
	public:
		CornellBox2024();
		~CornellBox2024();

		void Init() override;
	};
}
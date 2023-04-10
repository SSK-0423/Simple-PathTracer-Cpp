#pragma once
#include "Scene.hpp"

namespace PathTracer {
	class CornellBox : public Scene{
	public:
		CornellBox();
		~CornellBox();

		void Init() override;
	};
}
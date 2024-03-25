#pragma once
#include "Wildlands/Core/Base.h"

#include "Wildlands/Core/WL_KMCode.h"
#include <utility>

namespace Wildlands
{
	class  Input
	{
	public:
		static bool IsKeyDown(int keycode);
		static bool IsMouseButtonDown(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}

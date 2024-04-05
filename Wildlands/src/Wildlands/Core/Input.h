#pragma once

#include <glm/glm.hpp>
#include "Wildlands/Core/WL_KMCode.h"

namespace Wildlands
{
	class  Input
	{
	public:
		static bool IsKeyDown(KeyCode keycode);
		static bool IsMouseButtonDown(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}

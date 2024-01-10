#pragma once
#include "Wildlands/Core/Core.h"

#include "Wildlands/Core/WL_KMCode.h"
#include <utility>

namespace Wildlands
{
	class WL_API Input
	{
	public:
		inline static bool IsKeyDown(int keycode) { return s_Instance->IsKeyDownInternal(keycode); }
		inline static bool IsMouseButtonDown(int button) { return s_Instance->IsMouseButtonDownInternal(button); }
		inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionInternal(); }
		inline static float GetMouseX() { return s_Instance->GetMouseXInternal(); }
		inline static float GetMosueY() { return s_Instance->GetMouseYInternal(); }


	protected:
		virtual bool IsKeyDownInternal(int keycode) = 0;
		virtual bool IsMouseButtonDownInternal(int button) = 0;
		virtual std::pair<float, float> GetMousePositionInternal() = 0;
		virtual float GetMouseXInternal() = 0;
		virtual float GetMouseYInternal() = 0;

	private:
		static Input* s_Instance;
	};
}

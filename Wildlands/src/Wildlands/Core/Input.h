#pragma once
#include "Wildlands/Core/Base.h"

#include "Wildlands/Core/WL_KMCode.h"
#include <utility>

namespace Wildlands
{
	class  Input
	{
	protected:
		Input() = default;
	public:
		static Unique<Input> Craete();

		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

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
		static Unique<Input> s_Instance;
	};
}

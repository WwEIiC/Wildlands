#pragma once

#include "Wildlands/Core/Input.h"

namespace Wildlands
{
	class WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyDownInternal(int keycode) override;

		virtual bool IsMouseButtonDownInternal(int button) override;
		virtual std::pair<float, float> GetMousePositionInternal() override;
		virtual float GetMouseXInternal() override;
		virtual float GetMouseYInternal() override;
	};
}

#include "WLPCH.h"

#include "WindowsInput.h"
#include "Wildlands/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Wildlands 
{
	Unique<Input> Input::s_Instance = CreateUnique<WindowsInput>();

	bool WindowsInput::IsKeyDownInternal(int keycode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS;
	}
	bool WindowsInput::IsMouseButtonDownInternal(int button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	std::pair<float, float> WindowsInput::GetMousePositionInternal()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return std::pair<float, float>(static_cast<float>(xpos), static_cast<float>(ypos));
	}
	float WindowsInput::GetMouseXInternal()
	{
		auto[x, y] = GetMousePositionInternal();
		return x;
	}
	float WindowsInput::GetMouseYInternal()
	{
		auto[x, y] = GetMousePositionInternal();
		return y;
	}
}

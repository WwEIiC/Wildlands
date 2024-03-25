#include "WLPCH.h"

#include "Wildlands/Core/Input.h"
#include "Wildlands/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Wildlands 
{
	bool Input::IsKeyDown(int keycode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS;
	}
	bool Input::IsMouseButtonDown(int button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	std::pair<float, float> Input::GetMousePosition()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return std::pair<float, float>(static_cast<float>(xpos), static_cast<float>(ypos));
	}
	float Input::GetMouseX()
	{
		auto[x, y] = GetMousePosition();
		return x;
	}
	float Input::GetMouseY()
	{
		auto[x, y] = GetMousePosition();
		return y;
	}
}

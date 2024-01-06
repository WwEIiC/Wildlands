#include "WLPCH.h"
#include "WindowsWindow.h"

namespace Wildlands
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		WL_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowConstructData& data)
	{
		return new WindowsWindow(data);
	}


	WindowsWindow::WindowsWindow(const WindowConstructData& data)
	{
		Init(data);
	}
	WindowsWindow::~WindowsWindow()
	{
		Close();
	}


	void WindowsWindow::Init(const WindowConstructData& data)
	{
		m_Data.Title = data.Title;
		m_Data.Width = data.Width;
		m_Data.Height = data.Height;

		WL_CORE_INFO("Succeed to Creat window {0} ({1}, {2})", data.Title, data.Width, data.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			WL_CORE_ASSERT(success, "GLFW init failed!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)data.Width, (int)data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
	}
	void WindowsWindow::Close()
	{
		glfwDestroyWindow(m_Window);
	}


	void WindowsWindow::Update()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}
}

#include "WLPCH.h"

#include "WindowsWindow.h"
#include "Wildlands/Platforms/OpenGL/OpenGLContext.h"

#include "Wildlands/Events/ApplicationEvent.h"
#include "Wildlands/Events/KeyEvent.h"
#include "Wildlands/Events/MouseEvent.h"

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
		m_Data.VSync = data.VSync;

		WL_CORE_INFO("Succeed to Creat window {0} ({1}, {2})", data.Title, data.Width, data.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			WL_CORE_ASSERT(success, "GLFW init failed!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)data.Width, (int)data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		m_RenderContext = new OpenGLContext(m_Window);
		m_RenderContext->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(m_Data.VSync);

		//Set up Application events callbacks
		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				if (focused)
				{
					WindowFocusEvent event;
					data.EventCallback(event);
				}
				else
				{
					WindowLostFocusEvent event;
					data.EventCallback(event);
				}
			});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				WindowMovedEvent event(xpos, ypos);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				WindowCloseEvent event;
				data.EventCallback(event);
			});


		//Set up Mouse events callbacks
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonDownEvent event(button);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonUpEvent event(button);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});


		//Set up Key events callbacks
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyDownEvent event(key, 0);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyDownEvent event(key, 1);
						data.EventCallback(event);
						break;

					}
					case GLFW_RELEASE:
					{
						KeyUpEvent event(key);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypeEvent event(keycode);
				data.EventCallback(event);
			});
		
	}
	void WindowsWindow::Close()
	{
		glfwDestroyWindow(m_Window);
	}


	void WindowsWindow::Update()
	{
		glfwPollEvents();
		m_RenderContext->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enable)
	{
		glfwSwapInterval(enable);
		m_Data.VSync = enable;
	}
	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}

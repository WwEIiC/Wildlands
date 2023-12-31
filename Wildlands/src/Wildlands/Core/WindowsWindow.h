#pragma once

#include <GLFW/glfw3.h>
#include "Wildlands/Core/Window.h"

namespace Wildlands
{
	//The window class in the windows platform
	class WL_API WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowConstructData& data);
		virtual ~WindowsWindow();

		inline uint GetWidth() const override { return m_Data.Width; }
		inline uint GetHeight() const override { return m_Data.Height; }

		virtual void Update() override;
		
		inline virtual void SetEventCallback(const EventCallbackFunc& callback) override { m_Data.EventCallback = callback; }

	private:
		virtual void Init(const WindowConstructData& data);
		virtual void Close();

	private:
		//this struct will be used as the window's userpointer
		struct WindowData
		{
			std::string Title;
			uint Width, Height;
			EventCallbackFunc EventCallback;
		};
		WindowData m_Data;

		GLFWwindow* m_Window;
	};

}


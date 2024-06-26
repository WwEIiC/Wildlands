#pragma once

#include "Wildlands/Core/Window.h"
#include "Wildlands/Renderer/RenderContext.h"

#include <GLFW/glfw3.h>

namespace Wildlands
{
	//The window class in the windows platform
	class  WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowConstructData& data);
		virtual ~WindowsWindow();

		inline virtual void* GetNativeWindow() const override { return m_Window; }

		inline uint32_t GetWidth() const override { return m_Data.Width; }
		inline uint32_t GetHeight() const override { return m_Data.Height; }

		virtual void Update() override;
		
		virtual void SetVSync(bool enable) override;
		virtual bool IsVSync() const override;

		virtual bool IsFullScreen() const override { return m_IsFullScreen; }
		virtual void SetFullScreen() override;
		virtual void ExitFullScreen() override;

		inline virtual void SetEventCallback(const EventCallbackFunc& callback) override { m_Data.EventCallback = callback; }

	private:
		virtual void Init(const WindowConstructData& data);
		virtual void Close();

	private:
		//this struct will be used as the window's userpointer
		struct WindowData
		{
			std::string Title;
			uint32_t Width, Height;
			bool VSync;
			EventCallbackFunc EventCallback;
		};
		WindowData m_Data;

		GLFWwindow* m_Window;
		Unique<RenderContext> m_RenderContext;
		bool m_IsFullScreen = false;
	};

}


#pragma once
#include "Wildlands/Events/Event.h"

// define the window data struct and interface
namespace Wildlands
{
	struct WindowConstructData
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		bool VSync;

		WindowConstructData(const std::string& title = "Wildlands Engine",
			uint32_t width = 1280,
			uint32_t height = 720,
			bool vsync = true)
			: Title(title), Width(width), Height(height), VSync(vsync)
		{
		}
	};

	//The window interface(implementation in each platforms, only windows now)
	class  Window
	{
	public:
		using EventCallbackFunc = std::function<void(Event&)>;

		static Window* Create(const WindowConstructData& data = WindowConstructData());
		virtual ~Window() {}

		inline virtual void* GetNativeWindow() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Update() = 0;

		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;

		// set the event callback function
		virtual void SetEventCallback(const EventCallbackFunc& callback) = 0;
	};
}

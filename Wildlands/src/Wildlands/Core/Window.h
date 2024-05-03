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
			uint32_t width = 1600,
			uint32_t height = 900,
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

		static Unique<Window> Create(const WindowConstructData& data = WindowConstructData());
		virtual ~Window() {}

		inline virtual void* GetNativeWindow() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Update() = 0;

		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;

		virtual bool IsFullScreen() const = 0;
		virtual void SetFullScreen() = 0;
		virtual void ExitFullScreen() = 0;

		// set the event callback function
		virtual void SetEventCallback(const EventCallbackFunc& callback) = 0;
	};
}

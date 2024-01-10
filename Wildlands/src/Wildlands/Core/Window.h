#pragma once
#include "WLPCH.h"

#include "Wildlands/Core/Core.h"
#include "Wildlands/Events/Event.h"

// define the window data struct and interface
namespace Wildlands
{
	struct WindowConstructData
	{
		std::string Title;
		uint Width;
		uint Height;

		WindowConstructData(const std::string& title = "Wildlands Engine",
			uint width = 1280,
			uint height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	//The window interface(implementation in each platforms, only windows now)
	class WL_API Window
	{
	public:
		using EventCallbackFunc = std::function<void(Event&)>;

		static Window* Create(const WindowConstructData& data = WindowConstructData());
		virtual ~Window() {}

		inline virtual void* GetNativeWindow() const = 0;

		virtual uint GetWidth() const = 0;
		virtual uint GetHeight() const = 0;

		virtual void Update() = 0;

		// set the event callback function
		virtual void SetEventCallback(const EventCallbackFunc& callback) = 0;
	};
}

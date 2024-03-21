#include "WLPCH.h"
#include "Window.h"

#ifdef WL_PLATFORM_WINDOWS
#include "Wildlands/Platforms/Windows/WindowsWindow.h"
#endif


namespace Wildlands
{
	Unique<Window> Window::Create(const WindowConstructData& data)
	{
#ifdef WL_PLATFORM_WINDOWS
		return CreateUnique<WindowsWindow>(data);
#else
		WL_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}
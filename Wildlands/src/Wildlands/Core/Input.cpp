#include "WLPCH.h"
#include "Input.h"

#ifdef WL_PLATFORM_WINDOWS
#include "Wildlands/Platforms/Windows/WindowsInput.h"
#endif

namespace Wildlands
{
	Unique<Input> Input::s_Instance = Input::Craete();

	Unique<Input> Input::Craete()
	{
#ifdef WL_PLATFORM_WINDOWS
		return CreateUnique<WindowsInput>();
#else
		WL_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}
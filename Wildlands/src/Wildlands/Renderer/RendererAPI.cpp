#include "WLPCH.h"
#include "RendererAPI.h"

#include "Wildlands/Platforms/OpenGL/OpenGLRendererAPI.h"

namespace Wildlands
{
	RendererAPI::EAPI RendererAPI::s_API = RendererAPI::EAPI::OpenGL;

	Unique<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case RendererAPI::EAPI::None:
			WL_CORE_ASSERT(false, "RendererAPI is None");
			return nullptr;

		case RendererAPI::EAPI::OpenGL:
			return CreateUnique<OpenGLRendererAPI>();
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}
#include "WLPCH.h"
#include "RendererAPI.h"

#include "Wildlands/Platforms/OpenGL/OpenGLRendererAPI.h"

namespace Wildlands
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	Unique<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case RendererAPI::API::None:
			WL_CORE_ASSERT(false, "RendererAPI is None");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateUnique<OpenGLRendererAPI>();
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}
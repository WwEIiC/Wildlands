#include "WLPCH.h"
#include "RenderContext.h"

#include "Wildlands/Renderer/Renderer.h"
#include "Wildlands/Platforms/OpenGL/OpenGLContext.h"

namespace Wildlands
{
	Unique<RenderContext> RenderContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			WL_CORE_ASSERT(false, "RendererAPI is None");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateUnique<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}

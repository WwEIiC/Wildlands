#include "WLPCH.h"
#include "VertexArray.h"

#include "Wildlands/Renderer/Renderer.h"
#include "Wildlands/Platforms/OpenGL/OpenGLVertexArray.h"

namespace Wildlands
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::EAPI::None:
			WL_CORE_ASSERT(false, "RendererAPI is None");
			return nullptr;
		case RendererAPI::EAPI::OpenGL:
			return new OpenGLVertexArray();
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}

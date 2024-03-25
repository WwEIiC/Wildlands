#include "WLPCH.h"
#include "FrameBuffer.h"

#include "Wildlands/Renderer/Renderer.h"
#include "Wildlands/Platforms/OpenGL/OpenGLFrameBuffer.h"

namespace Wildlands{
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::EAPI::None:
			WL_CORE_ASSERT(false, "RendererAPI is None");
			return nullptr;

		case RendererAPI::EAPI::OpenGL:
			return CreateRef<OpenGLFrameBuffer>(spec);
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}

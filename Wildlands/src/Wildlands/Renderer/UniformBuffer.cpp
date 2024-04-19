#include "WLPCH.h"
#include "UniformBuffer.h"

#include "Wildlands/Renderer/Renderer.h"
#include "Wildlands/Platforms/OpenGL/OpenGLUniformBuffer.h"

namespace Wildlands
{

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::EAPI::None:    WL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::EAPI::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}

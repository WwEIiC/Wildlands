#include "WLPCH.h"
#include "Texture.h"

#include "Wildlands/Renderer/Renderer.h"
#include "Wildlands/Platforms/OpenGL/OpenGLTexture.h"

namespace Wildlands
{
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::EAPI::None:
			WL_CORE_ASSERT(false, "RendererAPI is None");
			return nullptr;

		case RendererAPI::EAPI::OpenGL:
			return std::make_shared<OpenGLTexture2D>(path);
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}

#include "WLPCH.h"
#include "RenderCommand.h"

#include "Wildlands/Platforms/OpenGL/OpenGLRendererAPI.h"

namespace Wildlands
{
	Unique<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}

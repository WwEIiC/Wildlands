#include "WLPCH.h"
#include "RenderCommand.h"

#include "Wildlands/Platforms/OpenGL/OpenGLRendererAPI.h"

namespace Wildlands
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}

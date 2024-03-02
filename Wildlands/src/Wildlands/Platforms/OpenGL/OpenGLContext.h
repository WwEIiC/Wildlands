#pragma once
#include "Wildlands/Renderer/RenderContext.h"

struct GLFWwindow;

namespace Wildlands
{
	class OpenGLContext : public RenderContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}

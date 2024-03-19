#include "WLPCH.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include "glad/glad.h"

namespace Wildlands
{

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		WL_CORE_ASSERT(windowHandle, "Can't get the GLFWwindow handle")
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		WL_CORE_ASSERT(status, "Failed to init Glad");

		WL_CORE_INFO("OpenGL Informations:");
		WL_CORE_INFO("		Vendor:	  {}", (const char*)glGetString(GL_VENDOR));
		WL_CORE_INFO("		Renderer: {}", (const char*)glGetString(GL_RENDERER));
		WL_CORE_INFO("		Version:  {}", (const char*)glGetString(GL_VERSION));

#ifdef WL_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		WL_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Wildlands requires at least OpenGL version 4.5");
#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}

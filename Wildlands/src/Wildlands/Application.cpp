#include "WLPCH.h"

#include "Application.h"
#include "Log.h"
#include "Events/ApplicationEvent.h"

namespace Wildlands
{
	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		Log::Init();
		WindowResizeEvent e(1280, 720);
		WL_TRACE(e);

		while (true)
		{

		}
	}
}

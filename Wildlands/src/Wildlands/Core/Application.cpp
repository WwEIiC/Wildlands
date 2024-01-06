#include "WLPCH.h"

#include "Application.h"
#include "Log.h"
#include "Wildlands/Events/ApplicationEvent.h"

namespace Wildlands
{
	Application::Application()
	{
		Log::Init();
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Running = true;
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_Window->Update();
		}
	}
}

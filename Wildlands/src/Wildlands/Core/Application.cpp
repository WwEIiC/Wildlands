#include "WLPCH.h"

#include "Application.h"
#include "Log.h"
#include "Wildlands/Events/ApplicationEvent.h"

namespace Wildlands
{
#define BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)
	Application::Application()
	{
		Log::Init();
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Running = true;

		m_Window->SetEventCallback(BIND_EVENT_FUNC(Application::OnEvents));
	}
	Application::~Application()
	{

	}

	void Application::OnEvents(Event& e)
	{
		//Call when event is received
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClose));

		WL_CORE_TRACE("{0}", e);
	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_Window->Update();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}
}

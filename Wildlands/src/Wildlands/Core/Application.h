#pragma once

#include "Wildlands/Core/Core.h"

#include "Wildlands/Events/Event.h"
#include "Wildlands/Core/Window.h"

namespace Wildlands
{
	class WL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvents(Event& e);

	private:
		bool OnWindowClose(class WindowCloseEvent& event);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running;
	};

	//Will be defined in Client
	Application* CreateApplication();
}


#pragma once

#include "Wildlands/Core/Core.h"

#include "Wildlands/Core/Window.h"
#include "Wildlands/Events/Event.h"
#include "Wildlands/Core/LayerStack.h"

namespace Wildlands
{
	class WL_API Application
	{
	public:
		Application();
		virtual ~Application() = default;

		void Run();

		void OnEvents(Event& e);

		// Push and Pop Layer to m_LayerStack
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
		void PushOverLayer(Layer* overlayer);
		void PopOverLayer(Layer* overlayer);
	private:
		bool OnWindowClose(class WindowCloseEvent& event);

	private:
		bool m_Running;
		std::unique_ptr<Window> m_Window;
		LayerStack m_LayerStack;
	};

	//Will be defined in Client
	Application* CreateApplication();
}


#include "WLPCH.h"

#include "Application.h"
#include "Wildlands/Core/Log.h"
#include "Wildlands/Events/ApplicationEvent.h"

#include <glad/glad.h>

namespace Wildlands
{
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Running = true;

		m_Window->SetEventCallback(BIND_EVENT_FUNC(Application::OnEvents));
	}

	void Application::OnEvents(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClose));

		//Handle event in each layer
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			WL_CORE_TRACE("Event handled by {0}", (*it)->GetName())
			if (e.Handled) { break; }
		}

		WL_CORE_TRACE("{0}", e)
	}

	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			//Render each layer
			for(Layer* layer : m_LayerStack)
			{
				layer->Update();
			}

			m_Window->Update();
		}
	}

#pragma region Push and Pop Layer
	void Application::PushLayer(Layer* layer) { m_LayerStack.PushLayer(layer); }
	void Application::PopLayer(Layer* layer) { m_LayerStack.PopLayer(layer); }
	void Application::PushOverLayer(Layer* overlayer) { m_LayerStack.PushOverlay(overlayer); }
	void Application::PopOverLayer(Layer* overlayer) { m_LayerStack.PopOverlay(overlayer); }
#pragma endregion

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}
}

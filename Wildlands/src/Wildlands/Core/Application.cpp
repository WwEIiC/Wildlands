#include "WLPCH.h"

#include "Application.h"
#include "Wildlands/Core/Log.h"
#include "Wildlands/Events/ApplicationEvent.h"
#include "Wildlands/Core/Input.h"

#include "Wildlands/Renderer/Renderer.h"

#include "Wildlands/Common/FileReader.h"

namespace Wildlands
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		WL_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Running = true;
		m_Window.reset(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FUNC(Application::OnEvents));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverLayer(m_ImGuiLayer);
	}

	void Application::OnEvents(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClose));

		//Handle event in each layer
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled) { break; }
		}
		//WL_CORE_TRACE("{0}", e)
	}

	void Application::Run()
	{
		while (m_Running)
		{
			//WLTODO: Move this to the Render thread.
			//Render each layers
			for(Layer* layer : m_LayerStack)
				layer->Update();

			//Render UI for each layers
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->UIRender();
			m_ImGuiLayer->End();

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

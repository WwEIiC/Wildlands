#include "WLPCH.h"

#include "Application.h"
#include "Wildlands/Core/Log.h"
#include "Wildlands/Events/ApplicationEvent.h"
#include "Wildlands/Core/Input.h"

#include "Wildlands/Renderer/Renderer.h"

#include <filesystem>
#include <GLFW/glfw3.h>

namespace Wildlands
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		WL_PROFILE_FUNCTION();

		WL_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Running = true;

		// Set working directory here
		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		m_Window = Window::Create(WindowConstructData(m_Specification.Name));
		m_Window->SetEventCallback(BIND_EVENT_FUNC(Application::OnEvents));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverLayer(m_ImGuiLayer);
	}

	Application::~Application()
	{
		WL_PROFILE_FUNCTION();

		Renderer::Destory();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvents(Event& e)
	{
		WL_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(Application::OnWindowResize));

		//Handle event in each layer
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled) { break; }
			(*it)->OnEvent(e);
		}
		//WL_CORE_TRACE("{0}", e)
	}

	void Application::Run()
	{
		WL_PROFILE_FUNCTION();

		while (m_Running)
		{
			WL_PROFILE_SCOPE("Running Loop");

			float time = (float)glfwGetTime();
			Timestep ts = time - m_LastFrameTime;
			m_LastFrameTime = time;

			//WLTODO: Move this to the Render thread.
			//Render each layers
			if (!m_Minimized)
			{
				{
					WL_PROFILE_SCOPE("Layers Update");
					for (Layer* layer : m_LayerStack)
						layer->Update(ts);
				}

				//Render UI for each layers
				m_ImGuiLayer->Begin();
				{
					WL_PROFILE_SCOPE("ImGuiLayers Update");
					for (Layer* layer : m_LayerStack)
						layer->UIRender();
				}
				m_ImGuiLayer->End();

			}
			m_Window->Update();
		}
	}

#pragma region Push and Pop Layer
	void Application::PushLayer(Layer* layer) 
	{
		WL_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer); 
		layer->Attach();
	}
	void Application::PopLayer(Layer* layer) 
	{
		WL_PROFILE_FUNCTION();

		layer->Detach();
		m_LayerStack.PopLayer(layer); 
	}
	void Application::PushOverLayer(Layer* overlayer) 
	{
		WL_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlayer); 
		overlayer->Attach();
	}
	void Application::PopOverLayer(Layer* overlayer) 
	{
		WL_PROFILE_FUNCTION();

		overlayer->Detach();
		m_LayerStack.PopOverlay(overlayer); 
	}
#pragma endregion

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		Close();
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& event)
	{
		WL_PROFILE_FUNCTION();

		if (event.GetWidth() == 0 || event.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());
		m_Minimized = false;
		return false;
	}
}

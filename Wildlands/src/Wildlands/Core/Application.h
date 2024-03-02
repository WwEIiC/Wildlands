#pragma once

#include "Wildlands/Core/Core.h"

#include "Wildlands/Core/Window.h"
#include "Wildlands/Core/LayerStack.h"
#include "Wildlands/ImGui/ImGuiLayer.h"
#include "Wildlands/Events/Event.h"

namespace Wildlands
{
	class WL_API Application
	{
	public:
		Application();
		virtual ~Application() = default;

		void Run();

		void OnEvents(Event& e);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }

		// Push and Pop Layer to m_LayerStack
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
		void PushOverLayer(Layer* overlayer);
		void PopOverLayer(Layer* overlayer);

	private:
		bool OnWindowClose(class WindowCloseEvent& event);
	private:
		//WLTODO:Make this a singleton(Template or not).
		static Application* s_Instance;

	private:
		bool m_Running;
		std::unique_ptr<Window> m_Window;

		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		unsigned int m_VAO, m_VBO, m_IBO;
	};

	//Will be defined in Client
	Application* CreateApplication();
}


#pragma once

#include "Wildlands/Core/Base.h"

#include "Wildlands/Core/Window.h"
#include "Wildlands/Core/Timestep.h"
#include "Wildlands/Core/LayerStack.h"
#include "Wildlands/ImGui/ImGuiLayer.h"

#include "Wildlands/Events/Event.h"

int main(int argc, char** argv);

namespace Wildlands
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			WL_CORE_ASSERT(index < Count, "ERROR");
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Wildlands Application";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class  Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void Close();

		void OnEvents(Event& e);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }

		inline ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }

		// Push and Pop Layer to m_LayerStack
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
		void PushOverLayer(Layer* overlayer);
		void PopOverLayer(Layer* overlayer);

	private:
		friend int ::main(int argc, char** argv);
		void Run();

		bool OnWindowClose(class WindowCloseEvent& event);
		bool OnWindowResize(class WindowResizeEvent& event);
	private:
		//WLTODO:Make this a singleton(Template or not).
		static Application* s_Instance;

	private:
		bool m_Running;
		bool m_Minimized = false;
		Unique<Window> m_Window;
		ApplicationSpecification m_Specification;

		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		float m_LastFrameTime;
	};

	//Will be defined in Client
	Application* CreateApplication(ApplicationCommandLineArgs args);
}


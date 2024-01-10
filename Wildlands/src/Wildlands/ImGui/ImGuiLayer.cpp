#include "WLPCH.h"

#include "imgui.h"
#include "Wildlands/Platforms/OpenGL/ImGuiRenderer_OpenGL.h"

#include "ImGuiLayer.h"
#include "Wildlands/Events/ApplicationEvent.h"
#include "Wildlands/Events/MouseEvent.h"
#include "Wildlands/Events/KeyEvent.h"

#include "Wildlands/Core/Application.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Wildlands
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}
	ImGuiLayer::~ImGuiLayer()
	{
	}
	void ImGuiLayer::Attach()
	{
		//Init ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch, Enable Docking

		//Set ImGui Style
		SetImGuiStyle();

		//Setup Renderer
		ImGui_ImplOpenGL3_Init("#version 410");
	}
	void ImGuiLayer::Detach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		//ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Update()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
		m_Time = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnWindowResize));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnMouseMoved));
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnMouseScrolled));
		dispatcher.Dispatch<MouseButtonDownEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonDown));
		dispatcher.Dispatch<MouseButtonUpEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonUp));
		dispatcher.Dispatch<KeyDownEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnKeyDown));
		dispatcher.Dispatch<KeyUpEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnKeyUp));
		dispatcher.Dispatch<KeyTypeEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnKeyType));
	}

#pragma region Event Handling
	bool ImGuiLayer::OnWindowResize(WindowResizeEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2(event.GetWidth(), event.GetHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		glViewport(0, 0, event.GetWidth(), event.GetHeight());
		return false;
	}
	bool ImGuiLayer::OnMouseMoved(MouseMovedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MousePos = ImVec2(event.GetX(), event.GetY());
		return false;
	}
	bool ImGuiLayer::OnMouseScrolled(MouseScrolledEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += event.GetXOffset();
		io.MouseWheel += event.GetYOffset();

		return false;//let it broadcast to other layers
	}
	bool ImGuiLayer::OnMouseButtonDown(MouseButtonDownEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[event.GetMouseButton()] = true;

		return false;//let it broadcast to other layers
	}
	bool ImGuiLayer::OnMouseButtonUp(MouseButtonUpEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[event.GetMouseButton()] = false;

		return false;//let it broadcast to other layers
	}
	bool ImGuiLayer::OnKeyDown(KeyDownEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[event.GetKeyCode()] = true;

		//also some special keys
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

		return false;
	}
	bool ImGuiLayer::OnKeyUp(KeyUpEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[event.GetKeyCode()] = false;

		return false;
	}
	bool ImGuiLayer::OnKeyType(KeyTypeEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		int keycode = event.GetKeyCode();
		if (keycode > 0 && keycode <= UINT16_MAX)
			io.AddInputCharacter((ushort)keycode);

		return false;
	}
#pragma endregion

	void ImGuiLayer::SetImGuiStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.37f, 0.36f, 0.36f, 102.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.10f, 0.10f, 0.10f, 171.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 255.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.45f, 0.45f, 0.45f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.55f, 0.55f, 0.55f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.46f, 0.46f, 0.46f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.17f, 0.17f, 0.17f, 0.95f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.78f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.45f, 0.45f, 0.45f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.51f, 0.51f, 0.51f, 0.70f);
		colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.21f, 0.86f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 0.97f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.40f, 0.13f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.45f, 1.00f, 0.85f, 0.35f);

		style.ScrollbarSize = 10;
		style.GrabMinSize = 8;
		style.WindowBorderSize = 0;
		style.WindowMenuButtonPosition = -1;
		style.WindowRounding = 8;
		style.FrameRounding = 6;
		style.ScrollbarRounding = 12;
	}
}

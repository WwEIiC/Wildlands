#include "WLPCH.h"
#include "ImGuiLayer.h"


#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <ImGuizmo.h>

#include "Wildlands/Core/Application.h"

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
		WL_PROFILE_FUNCTION();

		//Init ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		io.ConfigViewportsNoTaskBarIcon = true;

		//Set ImGui Style
		SetImGuiStyle();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		//Setup Renderer
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}
	void ImGuiLayer::Detach()
	{
		WL_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Update(Timestep ts)
	{
		
	}

	void ImGuiLayer::UIRender()
	{
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategory::EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategory::EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		WL_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}
	void ImGuiLayer::End()
	{
		WL_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		auto& app = Application::Get();
		io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()), 
			static_cast<float>(app.GetWindow().GetHeight()));

		//ImGui Renderering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//Multi-Viewport
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}


	void ImGuiLayer::SetImGuiStyle()
	{
		WL_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		// Fonts
		float fontSize = 18.0f;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/Fonts/Noto_Sans/NotoSans-Regular.ttf", fontSize);
		io.Fonts->AddFontFromFileTTF("assets/Fonts/Noto_Sans/NotoSans-Bold.ttf", fontSize);

		ImVec4* colors = style.Colors;
		// Frame
		colors[ImGuiCol_FrameBg]				= ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
		colors[ImGuiCol_FrameBgHovered]			= ImVec4(0.37f, 0.36f, 0.36f, 102.00f);
		colors[ImGuiCol_FrameBgActive]			= ImVec4(0.10f, 0.10f, 0.10f, 171.00f);

		// Title
		colors[ImGuiCol_TitleBgActive]			= ImVec4(0.20f, 0.20f, 0.20f, 255.00f);

		// Slider
		colors[ImGuiCol_SliderGrab]				= ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
		colors[ImGuiCol_SliderGrabActive]		= ImVec4(0.31f, 0.31f, 0.31f, 1.00f);

		// Header
		colors[ImGuiCol_Header]					= ImVec4(0.45f, 0.45f, 0.45f, 0.31f);
		colors[ImGuiCol_HeaderHovered]			= ImVec4(0.55f, 0.55f, 0.55f, 0.80f);
		colors[ImGuiCol_HeaderActive]			= ImVec4(0.09f, 0.09f, 0.09f, 1.00f);

		// Button
		colors[ImGuiCol_Button]					= ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
		colors[ImGuiCol_ButtonHovered]			= ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonActive]			= ImVec4(0.13f, 0.13f, 0.13f, 1.00f);

		// ResizeGrip
		colors[ImGuiCol_ResizeGrip]				= ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered]		= ImVec4(0.46f, 0.46f, 0.46f, 0.67f);
		colors[ImGuiCol_ResizeGripActive]		= ImVec4(0.17f, 0.17f, 0.17f, 0.95f);

		// SeparatorActive
		colors[ImGuiCol_SeparatorActive]		= ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
		colors[ImGuiCol_SeparatorHovered]		= ImVec4(0.50f, 0.50f, 0.50f, 0.78f);

		// Tab
		colors[ImGuiCol_Tab]					= ImVec4(0.21f, 0.21f, 0.21f, 0.86f);
		colors[ImGuiCol_TabHovered]				= ImVec4(0.45f, 0.45f, 0.45f, 0.80f);
		colors[ImGuiCol_TabActive]				= ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		colors[ImGuiCol_TabUnfocused]			= ImVec4(0.15f, 0.15f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive]		= ImVec4(0.19f, 0.19f, 0.19f, 1.00f);

		// Navigation
		colors[ImGuiCol_NavHighlight]			= ImVec4(1.00f, 0.40f, 0.13f, 1.00f);

		colors[ImGuiCol_TextSelectedBg]			= ImVec4(0.45f, 1.00f, 0.85f, 0.35f);
		colors[ImGuiCol_CheckMark]				= ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_DockingPreview]			= ImVec4(0.51f, 0.51f, 0.51f, 0.70f);

		style.ScrollbarSize						= 10;
		style.GrabMinSize						= 8;
		style.WindowBorderSize					= 0;
		style.WindowMenuButtonPosition			= -1;

		// Rounding
		style.WindowRounding					= 8;
		style.TabRounding						= 4;
		style.FrameRounding						= 4;
		style.PopupRounding						= 4;
		style.ScrollbarRounding					= 12;

		// Docking
		style.DockingSeparatorSize				= 0;
	}
}

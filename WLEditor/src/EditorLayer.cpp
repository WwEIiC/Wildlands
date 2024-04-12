#include "EditorLayer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Wildlands/ECS/SceneSerializer.h"
#include "Wildlands/Utils/PlatformUtils.h"

namespace Wildlands
{
    EditorLayer::EditorLayer()
        : Layer("Sandbox2D")
    {
    }


    void EditorLayer::Attach()  
    {
        WL_PROFILE_FUNCTION();
        WL_INFO("Sandbox2D Layer Attached");
        m_Texture = Texture2D::Create("./assets/Textures/Checkerboard.png");

        FrameBufferSpecification framebufferSpec;
        framebufferSpec.Width = 1280;
        framebufferSpec.Height = 720;
        m_FrameBuffer = FrameBuffer::Create(framebufferSpec);

        m_ActiveScene = CreateRef<Scene>();
        m_HierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::Detach()
    {
        WL_PROFILE_FUNCTION();
        WL_INFO("Sandbox2D Layer Detached");
    }

    void EditorLayer::Update(Timestep ts)
    {
        WL_PROFILE_FUNCTION();

        Renderer2D::ResetStats();
        {
            WL_PROFILE_SCOPE("Render Pre");
            m_FrameBuffer->Bind();
            RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
            RenderCommand::Clear();
        }

        {
            WL_PROFILE_SCOPE("Renderer Draw");

			// Scene Update
			m_ActiveScene->Update(ts);

            m_FrameBuffer->UnBind();
        }
    }

    void EditorLayer::UIRender()
    {
        WL_PROFILE_FUNCTION();

        static bool dockOpen = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", &dockOpen, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float windowMinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        style.WindowMinSize.x = windowMinSizeX;

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Settings")) { m_HierarchyPanel.m_ShowSettingsWindow = true; }
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) { NewScene(); }
                if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S")) { SaveSceneAs(); }
                if (ImGui::MenuItem("Open...", "Ctrl+O")) { OpenScene(); }
                if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Scene Hierarchy Panel
        m_HierarchyPanel.OnImGuiRender();
        ImGui::ShowDemoWindow();

        ImGui::Begin("Render Stats");
        auto stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Vertices: %d", stats.GetVertexCount());
		ImGui::Text("Indices: %d", stats.GetIndexCount());
        ImGui::End();

        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport");
        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused || !m_ViewportHovered);

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        if (glm::vec2(viewportSize.x, viewportSize.y) != m_ViewportSize)
        {
            m_FrameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
            m_ViewportSize = { viewportSize.x, viewportSize.y };

            m_ActiveScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
        }

        // ImGuiCoord :: (0, 0) is the left top and (1, 1) is the right bottom
        // OpenGLCoord:: (0, 0) is the left bottom and (1, 1) is the right top
        ImGui::Image((void*)m_FrameBuffer->GetColorAttachmentRendererID(), ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();//"Viewport"
        ImGui::PopStyleVar();


        ImGui::End();//""Dockspace"
    }

    void EditorLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyDownEvent>(BIND_EVENT_FUNC(EditorLayer::OnKeyDownEvent));
    }

    bool EditorLayer::OnKeyDownEvent(KeyDownEvent& e)
    {
        // only deal with the shortcut now.
        if (e.GetRepeatCount() > 0) { return false; }


        bool ctrl = Input::IsKeyDown(Key::LeftControl) || Input::IsKeyDown(Key::RightControl);
        bool shift = Input::IsKeyDown(Key::LeftShift) || Input::IsKeyDown(Key::RightShift);
        switch (e.GetKeyCode())
        {
			case Key::N:
			{
				if (ctrl)
					NewScene();
				break;
			}
			case Key::S :
			{
				if (ctrl && shift)
					SaveSceneAs();
				break;
			}
			case Key::O:
			{
				if (ctrl)
					OpenScene();
				break;
			}
        }
    }
    void EditorLayer::NewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_HierarchyPanel.SetContext(m_ActiveScene);
    }
    void EditorLayer::SaveSceneAs()
    {
		auto filePath = FileDialogs::SaveFile("Wildlands Scene (*.wls)\0*.wls\0");
		if (filePath)
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(*filePath);
		}
    }
    void EditorLayer::OpenScene()
    {
		auto filePath = FileDialogs::OpenFile("Wildlands Scene (*.wls)\0*.wls\0");
		if (filePath)
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_HierarchyPanel.SetContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(*filePath);
		}
    }
}


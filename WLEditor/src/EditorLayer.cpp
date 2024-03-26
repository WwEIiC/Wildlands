#include "EditorLayer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Wildlands
{
    EditorLayer::EditorLayer()
        : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
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
    }

    void EditorLayer::Detach()
    {
        WL_PROFILE_FUNCTION();
        WL_INFO("Sandbox2D Layer Detached");
    }

    void EditorLayer::Update(Timestep ts)
    {
        WL_PROFILE_FUNCTION();
        //Camera
        if (m_ViewportFocused)
            m_CameraController.OnUpdate(ts);

        Renderer2D::ResetStats();

        {
            WL_PROFILE_SCOPE("Render Pre");
            m_FrameBuffer->Bind();
            RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
            RenderCommand::Clear();
        }

        {
            WL_PROFILE_SCOPE("Renderer Draw");

            Renderer2D::BeginScene(m_CameraController.GetCamera());
            Renderer2D::DrawQuad(m_Position, m_Size, m_Color);
            Renderer2D::DrawQuad(m_Position, m_Size, m_Color);
            Renderer2D::DrawRotatedQuad(m_Position + glm::vec3{ -0.8f, 0.8f, 0.0f }, m_Size, 45.f, { 0.2, 0.2, 0.8, 1.0 });

            glm::vec3 offset = { 1.8f, -0.5f, 0.0f };
            Renderer2D::DrawQuad(m_Position + offset, m_Size, m_Texture);
            Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 30.f, 30.f }, m_Texture);

            //Renderer2D::EndScene();

            //Renderer2D::BeginScene(m_CameraController.GetCamera());

            /*float quadOffset = 0.1f;
            int quadNum = 40;
            float halfLength = ((quadNum - 1) * quadOffset + quadNum ) * 0.5f;
            for (float x = -halfLength; x < halfLength; x += (1 + quadOffset))
            {
                for (float y = -halfLength; y < halfLength; y += (1 + quadOffset))
                {
                    glm::vec4 quadColor = { (x + halfLength) / (2 * halfLength), 0.4f, (y + halfLength) / (2 * halfLength), 0.6f };
                    Renderer2D::DrawQuad({ x, y, 0.0f }, { 1.0f, 1.0f }, quadColor);
                }
            }*/
            Renderer2D::EndScene();
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

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
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
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }



        ImGui::Begin("Settings");

        ImGui::SliderFloat3("Position", glm::value_ptr(m_Position), -5.f, 5.f);
        ImGui::SliderFloat2("Size", glm::value_ptr(m_Size), -5.f, 5.f);
        ImGui::ColorEdit4("Color", glm::value_ptr(m_Color));

        auto& stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.drawCalls);
        ImGui::Text("Quad Count: %d", stats.quadCount);
        ImGui::Text("Vertices: %d", stats.GetVertexCount());
        ImGui::Text("Indices: %d", stats.GetIndexCount());

        ImGui::End();//"settings"

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

            m_CameraController.ResizeViewport(viewportSize.x, viewportSize.y);
        }
        //ImGuiCoord :: (0, 0) is the left top and (1, 1) is the right bottom
        //OpenGLCoord:: (0, 0) is the left bottom and (1, 1) is the right top
        ImGui::Image((void*)m_FrameBuffer->GetColorAttachmentRendererID(), ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
        //ImGui::Image((void*)m_FrameBuffer->GetColorAttachmentRendererID(), ImVec2(1280, 720), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();//"Viewport"
        ImGui::PopStyleVar();


        ImGui::End();//""Dockspace"
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_CameraController.OnEvent(event);
    }
}


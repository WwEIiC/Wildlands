#include "EditorLayer.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

#include "Wildlands/ECS/SceneSerializer.h"
#include "Wildlands/Utils/PlatformUtils.h"

#include "Wildlands/Math/Math.h"

namespace Wildlands
{
    extern const std::filesystem::path g_AssetPath;

    EditorLayer::EditorLayer()
        : Layer("Sandbox2D")
    {
    }


    void EditorLayer::Attach()  
    {
        WL_PROFILE_FUNCTION();
        WL_INFO("WLEditor Layer Attached");

        m_PlayButtonIcon = Texture2D::Create("resources/icons/PlayButtonIcon.png");
        m_StopButtonIcon = Texture2D::Create("resources/icons/StopButtonIcon.png");

        FrameBufferSpecification framebufferSpec;
        framebufferSpec.Attachments = {
            FrameBufferTextureFormat::RGBA8,
            FrameBufferTextureFormat::RED_UINTEGER,
            FrameBufferTextureFormat::Depth
        };
        framebufferSpec.Width = 1600;
        framebufferSpec.Height = 900;
        m_FrameBuffer = FrameBuffer::Create(framebufferSpec);

        m_ActiveScene = CreateRef<Scene>();

        auto commandLineArgs = Application::Get().GetCommandLineArgs();
        if (commandLineArgs.Count > 1)
        {
            auto sceneFilePath = commandLineArgs[1];
            SceneSerializer serializer(m_ActiveScene);
            serializer.Deserialize(sceneFilePath);
        }

        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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

            // clear the entityID attachment with EntityNull.
            uint32_t value = (uint32_t)Entity::EntityNull;
            m_FrameBuffer->ClearColorAttachment(1, &value);
        }

        {
            WL_PROFILE_SCOPE("Renderer Draw");

			// Scene Update
            switch (m_SceneState)
            {
            case ESceneState::Edit:
            {
                m_EditorCamera.Update(ts);

                m_ActiveScene->UpdateEditor(ts, m_EditorCamera);
                break;
            }
            case ESceneState::Play:
            {
                m_ActiveScene->UpdateRuntime(ts);
                break;
            }
            }
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
                if (ImGui::MenuItem("Settings")) { m_SceneHierarchyPanel.m_ShowSettingsWindow = true; }
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) { NewScene(); }
                if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S")) { SaveSceneAs(); }
                if (ImGui::MenuItem("Open...", "Ctrl+O")) { OpenScene(); }
                if (ImGui::MenuItem("FullScreen")) { Application::Get().GetWindow().SetFullScreen(); }
                if (ImGui::MenuItem("ExitFullScreen")) { Application::Get().GetWindow().ExitFullScreen(); }
                if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Panels
        m_SceneHierarchyPanel.OnImGuiRender();
        m_ContentBrowserPanel.OnImGuiRender();

        ImGui::Begin("Render Stats");

        std::string name = "None";
        if (m_HoveredEntity)
            name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
        ImGui::Text("Hovered Entity: %s", name.c_str());

        auto stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Vertices: %d", stats.GetVertexCount());
		ImGui::Text("Indices: %d", stats.GetIndexCount());
        ImGui::End();

        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar);

        // set up viewport data.
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused && !m_ViewportHovered);

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        if (glm::vec2(viewportSize.x, viewportSize.y) != m_ViewportSize)
        {
            m_FrameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
            m_ViewportSize = { viewportSize.x, viewportSize.y };

            m_EditorCamera.SetViewportSize(viewportSize.x, viewportSize.y);
            m_ActiveScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
        }

        //-- ImGuiCoord :: (0, 0) is the left top and (1, 1) is the right bottom
        //-- OpenGLCoord:: (0, 0) is the left bottom and (1, 1) is the right top
        uint64_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const wchar_t* path = (const wchar_t*)payload->Data;
                OpenScene(std::filesystem::path(g_AssetPath) / path);
            }
            ImGui::EndDragDropTarget();
        }

        // ImGuizmo
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity && m_GizmoType != -1)
        {
            // Set it to Perspective now.
			ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y,
                m_ViewportBounds[1].x - m_ViewportBounds[0].x,
                m_ViewportBounds[1].y - m_ViewportBounds[0].y);

            // Camera - Runtime
            //Entity primaryCameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
            //const auto& camera = primaryCameraEntity.GetComponent<CameraComponent>().Camera;
            //const glm::mat4& cameraProjection = camera.GetProjection();
            //glm::mat4 cameraView = glm::inverse(primaryCameraEntity.GetComponent<TransformComponent>().GetTransform());

            //Editor Camera
            const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();


            // Selected Entity
            auto& transformComp = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = transformComp.GetTransform();

            // Snapping
            bool snap = Input::IsKeyDown(Key::LeftControl);
            float snapValue = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 15.0f : 0.5f;
            float snapValues[3] = { snapValue, snapValue, snapValue };

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)m_GizmoType, (ImGuizmo::MODE)m_GizmoMode, glm::value_ptr(transform),
                nullptr, snap ? snapValues : nullptr);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 position, rotation, scale;
                Math::DecomposeTransform(transform, position, rotation, scale);

                glm::vec3 deltaRotation = rotation - transformComp.Rotation;
                transformComp.Position = position;
                transformComp.Rotation += deltaRotation;
                transformComp.Scale = scale;
            }
        }

        ImGui::End();//"Viewport"
        ImGui::PopStyleVar();

        UI_Toolbar();

        ImGui::End();//""Dockspace"
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_EditorCamera.OnEvent(event);

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyDownEvent>(BIND_EVENT_FUNC(EditorLayer::OnKeyDownEvent));
        dispatcher.Dispatch<MouseButtonDownEvent>(BIND_EVENT_FUNC(EditorLayer::OnMouseButtonDownEvent));
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////// UI Functions ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditorLayer::UI_Toolbar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 2});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2{0, 0});
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
        auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;
        ImGui::Begin("Toolbar", nullptr, flags);
        float buttonSize = ImGui::GetWindowHeight() - 4.f;
        ImGui::SameLine((ImGui::GetContentRegionMax().x - buttonSize) * 0.5f);
        Ref<Texture2D> icon = m_SceneState == ESceneState::Edit ? m_PlayButtonIcon : m_StopButtonIcon;
        if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2{buttonSize, buttonSize}, ImVec2(0, 0), ImVec2(1, 1), 0))
        {
            if (m_SceneState == ESceneState::Edit)
                OnScenePlay();
            else if (m_SceneState == ESceneState::Play)
                OnSceneStop();
        }
        ImGui::End();

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////// Events Functions ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditorLayer::OnKeyDownEvent(KeyDownEvent& e)
    {
        // only deal with the shortcut now.
        if (e.GetRepeatCount() > 0) { return false; }


        bool ctrl = Input::IsKeyDown(Key::LeftControl) || Input::IsKeyDown(Key::RightControl);
        bool shift = Input::IsKeyDown(Key::LeftShift) || Input::IsKeyDown(Key::RightShift);
        bool IsMouseOrGizmoUsing = ImGuizmo::IsUsing() || Input::IsMouseButtonDown(Mouse::ButtonRight);
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

            // Gizmos
            case Key::Q: { if (!IsMouseOrGizmoUsing) { m_GizmoType = -1; } break; }
            case Key::W: { if (!IsMouseOrGizmoUsing) { m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; } break; }
            case Key::E: { if (!IsMouseOrGizmoUsing) { m_GizmoType = ImGuizmo::OPERATION::ROTATE; } break; }
            case Key::R: { if (!IsMouseOrGizmoUsing) { m_GizmoType = ImGuizmo::OPERATION::SCALE; } break; }
            case Key::V: { if (!IsMouseOrGizmoUsing) { m_GizmoMode = !m_GizmoMode; } break; }

        }
        return false;
    }

    bool EditorLayer::OnMouseButtonDownEvent(MouseButtonDownEvent& e)
    {
        if (e.GetMouseButton() == Mouse::ButtonLeft)
        {
            if (m_ViewportHovered && !ImGuizmo::IsOver())
            {
                //-- Mouse Picking.
                //-- Calculate the mouse position relative to the viewport image.
				auto [mouseX, mouseY] = ImGui::GetMousePos();
				mouseX -= m_ViewportBounds[0].x;
				mouseY = m_ViewportBounds[1].y - mouseY;//becouse the different between textureCoord and imguiCoord(flip the y).
				if (0 <= mouseX && mouseX <= m_ViewportSize.x && 0 <= mouseY && mouseY <= m_ViewportSize.y)
				{
                    m_FrameBuffer->Bind();
					uint32_t pixelData= m_FrameBuffer->ReadPixelAsUInt(1, (int)mouseX, (int)mouseY);
					m_HoveredEntity = Entity((entt::entity)pixelData, m_ActiveScene.get());
                    m_FrameBuffer->UnBind();
				}

				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			}
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////// Scene Functions ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditorLayer::NewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }
    void EditorLayer::SaveSceneAs()
    {
		auto filePath = FileDialogs::SaveFile("Wildlands Scene (*.wls)\0*.wls\0");
		if (!filePath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filePath);
		}
    }
    void EditorLayer::OpenScene()
    {
		auto filePath = FileDialogs::OpenFile("Wildlands Scene (*.wls)\0*.wls\0");
		if (!filePath.empty())
            OpenScene(filePath);
    }
    void EditorLayer::OpenScene(std::filesystem::path path)
    {
        m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		SceneSerializer serializer(m_ActiveScene);
		serializer.Deserialize(path.string());
    }

    void EditorLayer::OnScenePlay()
    {
        m_SceneState = ESceneState::Play;
    }
    void EditorLayer::OnSceneStop()
    {
        m_SceneState = ESceneState::Edit;
    }
}


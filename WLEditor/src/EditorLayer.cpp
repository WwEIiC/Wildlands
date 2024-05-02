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

        m_PlayButtonIcon    = Texture2D::Create("resources/icons/PlayButtonIcon.png");
        m_SimulateButtonIcon= Texture2D::Create("resources/icons/SimulateButtonIcon.png");
        m_StopButtonIcon    = Texture2D::Create("resources/icons/StopButtonIcon.png");

        FrameBufferSpecification framebufferSpec;
        framebufferSpec.Attachments = {
            FrameBufferTextureFormat::RGBA8,
            FrameBufferTextureFormat::RED_UINTEGER,
            FrameBufferTextureFormat::Depth
        };
        framebufferSpec.Width = 1600;
        framebufferSpec.Height = 900;
        m_FrameBuffer = FrameBuffer::Create(framebufferSpec);

        m_EditorScene = CreateRef<Scene>();
        m_ActiveScene = m_EditorScene;

        auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
        if (commandLineArgs.Count > 1)
        {
            auto sceneFilePath = commandLineArgs[1];
            OpenScene(sceneFilePath);
        }

        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

        Renderer2D::SetLineWidth(0.95f);
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
				case SceneState::Edit:
				{
					m_EditorCamera.Update(ts);

					m_ActiveScene->UpdateEditor(ts, m_EditorCamera);
					break;
				}
				case SceneState::Play:
				{
					m_ActiveScene->UpdateRuntime(ts);
					break;
				}
                case SceneState::Simulate:
                {
                    m_EditorCamera.Update(ts);

                    m_ActiveScene->UpdateSimulation(ts, m_EditorCamera);
                    break;
                }
            }

            OverlayRender();
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
                if (ImGui::MenuItem("Save", "Ctrl+S")) { SaveScene(); }
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

        auto stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Vertices: %d", stats.GetVertexCount());
		ImGui::Text("Indices: %d", stats.GetIndexCount());
        ImGui::End();

        ImGui::Begin("Scene Settings");
        ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);
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
        Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportHovered);

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
        if (m_SceneState == SceneState::Edit && selectedEntity && m_GizmoType != -1)
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
        if (m_SceneState == SceneState::Edit)
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
        bool toolbarEnabled = (bool)m_ActiveScene;

        ImVec4 tintColor = ImVec4(1, 1, 1, 1);
        if (!toolbarEnabled)
            tintColor.w = 0.5f;
        float buttonSize = ImGui::GetWindowHeight() - 4.f;

        {
            Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_PlayButtonIcon : m_StopButtonIcon;
            ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (buttonSize * 0.5f));
            if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(buttonSize, buttonSize), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
            {
                if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
                    OnScenePlay();
                else if (m_SceneState == SceneState::Play)
                    OnSceneStop();
            }
        }
		ImGui::SameLine();
		{
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_SimulateButtonIcon : m_StopButtonIcon;		//ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(buttonSize, buttonSize), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimulate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
			}
		}

        ImGui::End();
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
    }

    void EditorLayer::OverlayRender()
    {
        if (m_SceneState == SceneState::Play)
        {
            Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
            if (!camera) { return; }

            Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera.GetProjection(), camera.GetComponent<TransformComponent>().GetTransform());
        }
        else
            Renderer2D::BeginScene(m_EditorCamera);

        if (m_ShowPhysicsColliders)
        {
            // Box Colliders
            {
                auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
                for (auto entity : view)
                {
                    auto [transformComp, bc2dComp] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

                    glm::vec3 translation = transformComp.Position + glm::vec3(bc2dComp.Offset, 0.001f);
                    glm::vec3 scale = transformComp.Scale * glm::vec3(bc2dComp.Size, 1.0f);

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                        * glm::rotate(glm::mat4(1.0f), transformComp.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
                        * glm::scale(glm::mat4(1.0f), scale);

                    Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
                }
            }

            // Circle Colliders
            {
                auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
                for (auto entityID : view)
                {
                    auto [transformComp, cc2dComp] = view.get<TransformComponent, CircleCollider2DComponent>(entityID);
                    glm::vec3 translation = transformComp.Position + glm::vec3(cc2dComp.Offset, 0.001f);
                    glm::vec3 scale = transformComp.Scale * glm::vec3(cc2dComp.Radius * 2.0f);

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                        * glm::scale(glm::mat4(1.0f), scale);

                    Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.02f);
                }
            }
        }

        // Draw selected entity outline 
        if (Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity())
        {
            TransformComponent transformComp = selectedEntity.GetComponent<TransformComponent>();

            Renderer2D::DrawRect(transformComp.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
        }
        Renderer2D::EndScene();
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////// Events Functions ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditorLayer::OnKeyDownEvent(KeyDownEvent& e)
    {
        // only deal with the shortcut now.
        if (e.IsRepeat()) { return false; }


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
                if (ctrl)
                {
                    if (shift) { SaveSceneAs(); }
					else { SaveScene(); }
                }
				break;
			}
			case Key::O:
			{
				if (ctrl)
					OpenScene();
				break;
			} 
            case Key::D:
			{
				if (ctrl)
					OnDuplicateEntity();
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
					m_MouseClickedEntity = Entity((entt::entity)pixelData, m_ActiveScene.get());
                    m_FrameBuffer->UnBind();
				}

				m_SceneHierarchyPanel.SetSelectedEntity(m_MouseClickedEntity);
			}
        }
        return false;
    }

    void EditorLayer::OnDuplicateEntity()
    {
        if (m_SceneState != SceneState::Edit) { return; }

        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity) { m_ActiveScene->DupilcateEntity(selectedEntity); }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////// Scene Functions ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditorLayer::NewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        m_EditorScenePath = std::filesystem::path();
    }
    void EditorLayer::SaveScene()
    {
        if (!m_EditorScenePath.empty())
            SerializeScene(m_ActiveScene, m_EditorScenePath);
        else
            SaveSceneAs();
    }
    void EditorLayer::SaveSceneAs()
    {
		auto filePath = FileDialogs::SaveFile("Wildlands Scene (*.wls)\0*.wls\0");
		if (!filePath.empty())
		{
            SerializeScene(m_ActiveScene, filePath);
            m_EditorScenePath = filePath;
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
        if (m_SceneState != SceneState::Edit) { OnSceneStop(); }

        if (path.extension().string() != ".wls")
        {
            WL_CORE_WARN("Could not load {0} - not a scene file", path.filename().string());
            return;
        }

        Ref<Scene> newScene = CreateRef<Scene>();
        SceneSerializer serializer(newScene);
        if (serializer.Deserialize(path.string()))
        {
            m_EditorScene = newScene;
            m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_SceneHierarchyPanel.SetContext(m_EditorScene);

            m_ActiveScene = m_EditorScene;
            m_EditorScenePath = path;
        }
    }

    void EditorLayer::SerializeScene(Ref<Scene> scene, std::filesystem::path path)
    {
			SceneSerializer serializer(scene);
			serializer.Serialize(path.string());
    }

    void EditorLayer::OnScenePlay()
    {
        if (m_SceneState == SceneState::Simulate) { OnSceneStop(); }

        m_SceneState = SceneState::Play;

        m_ActiveScene = Scene::Copy(m_EditorScene);

        m_ActiveScene->OnRuntimeStart();
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }
    void EditorLayer::OnSceneSimulate()
    {
        if (m_SceneState == SceneState::Play) { OnSceneStop(); }

        m_SceneState = SceneState::Simulate;

        m_ActiveScene = Scene::Copy(m_EditorScene);

        m_ActiveScene->OnSimulationStart();
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }
    void EditorLayer::OnSceneStop()
    {
        WL_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate, "Scene couldn't stop in editor state");

        m_SceneState = SceneState::Edit;

        if (m_SceneState == SceneState::Play)
            m_ActiveScene->OnRuntimeStop();
        else if (m_SceneState == SceneState::Simulate)
            m_ActiveScene->OnSimulationStop();

		m_ActiveScene = m_EditorScene;
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }
}


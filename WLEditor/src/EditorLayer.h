#pragma once
#include "Wildlands.h"
#include "Wildlands/Renderer/EditorCamera.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

namespace Wildlands
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() override = default;

		virtual void Attach() override;
		virtual void Detach() override;

		virtual void Update(Timestep ts) override;
		virtual void UIRender() override;

		virtual void OnEvent(Event& event) override;
	private:
		void UI_Toolbar();
		void OverlayRender();
	private:
		// Events
		bool OnKeyDownEvent(KeyDownEvent& e);
		bool OnMouseButtonDownEvent(MouseButtonDownEvent& e);

		void OnDuplicateEntity();
	private:
		// Project
		void NewProject();
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();
	private:
		// Scene
		void NewScene();
		void SaveScene();
		void SaveSceneAs();
		void OpenScene();
		void OpenScene(std::filesystem::path path);

		void SerializeScene(Ref<Scene> scene, std::filesystem::path path);

		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();
		void OnScenePause();
	private:
		Ref<FrameBuffer> m_FrameBuffer;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		/// <summary> left-top and right-bottom </summary>
		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
	private:
		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		EditorCamera m_EditorCamera;

		int m_GizmoType = -1;
		bool m_GizmoMode = false; // false  = local, true = world.
		Entity m_MouseClickedEntity;
		
		bool m_ShowPhysicsColliders = false;

		enum class SceneState
		{
			Edit = 0,
			Play = 1,
			Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;
		float m_lastFrameTime;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		Unique<ContentBrowserPanel> m_ContentBrowserPanel;

		Ref<Texture2D> m_PlayButtonIcon, m_SimulateButtonIcon, m_StopButtonIcon;
		Ref<Texture2D> m_PauseButtonIcon, m_StepforwardButtonIcon;
	};
}


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
		// UI
		void UI_Toolbar();
	private:
		// Events
		bool OnKeyDownEvent(KeyDownEvent& e);
		bool OnMouseButtonDownEvent(MouseButtonDownEvent& e);

		void OnDuplicateEntity();
	private:
		// Scene
		void NewScene();
		void SaveScene();
		void SaveSceneAs();
		void OpenScene();
		void OpenScene(std::filesystem::path path);

		void SerializeScene(Ref<Scene> scene, std::filesystem::path path);

		void OnScenePlay();
		void OnSceneStop();
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
		Entity m_HoveredEntity;
		
		enum class SceneState
		{
			Edit = 0,
			Play = 1
		};
		SceneState m_SceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		Ref<Texture2D> m_PlayButtonIcon, m_StopButtonIcon;
	};
}


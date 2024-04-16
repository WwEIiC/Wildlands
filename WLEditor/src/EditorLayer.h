#pragma once
#include "Wildlands.h"
#include "Wildlands/Renderer/EditorCamera.h"
#include "Panels/SceneHierarchyPanel.h"

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
		bool OnKeyDownEvent(KeyDownEvent& e);
		bool OnMouseButtonDownEvent(MouseButtonDownEvent& e);
		void NewScene();
		void SaveSceneAs();
		void OpenScene();
	private:
		Ref<Texture2D> m_Texture;
		Ref<FrameBuffer> m_FrameBuffer;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		/// <summary> left-top and right-bottom </summary>
		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
	private:
		Ref<Scene> m_ActiveScene;

		EditorCamera m_EditorCamera;

		SceneHierarchyPanel m_HierarchyPanel;
		int m_GizmoType = -1;
		bool m_GizmoMode = false; // false  = local, true = world.
		Entity m_HoveredEntity;
	};
}


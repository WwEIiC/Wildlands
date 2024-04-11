#pragma once
#include "Wildlands.h"
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
		bool OnKeyDownEvent(KeyTypeEvent& e);
	private:
		Ref<Texture2D> m_Texture;
		Ref<FrameBuffer> m_FrameBuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_CameraEntity;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_Size = { 1.0f, 1.0f };

	private:
		SceneHierarchyPanel m_HierarchyPanel;
	};
}


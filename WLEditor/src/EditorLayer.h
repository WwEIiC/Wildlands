#pragma once
#include "Wildlands.h"

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
		OrthographicCameraController m_CameraController;
		Ref<Texture2D> m_Texture;
		Ref<FrameBuffer> m_FrameBuffer;

        bool showQuad = false;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		glm::vec4 m_Color = { 0.8f, 0.2f, 0.8f, 1.0f };
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_Size = { 1.0f, 1.0f };
	};
}


#pragma once
#include "Wildlands.h"

class Sandbox2D : public Wildlands::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() override = default;

	virtual void Attach() override;
	virtual void Detach() override;

	virtual void Update(Wildlands::Timestep ts) override;
	virtual void UIRender() override;

	virtual void OnEvent(Wildlands::Event& event) override;
private:
	Wildlands::OrthographicCameraController m_CameraController;
	Wildlands::Ref<Wildlands::Texture2D> m_Texture;

	glm::vec4 m_Color = { 0.8f, 0.2f, 0.8f, 1.0f };
	glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
	glm::vec2 m_Size = { 1.0f, 1.0f };
};


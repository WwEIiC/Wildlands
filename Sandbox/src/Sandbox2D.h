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
	Wildlands::Ref<Wildlands::Texture2D> texture;

	glm::vec4 color = {0.8f, 0.2f, 0.8f, 1.0f};
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec2 size = { 1.0f, 1.0f };
};


#include "Sandbox2D.h"
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "Wildlands/Platforms/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}


void Sandbox2D::Attach()
{
	WL_PROFILE_FUNCTION();
	WL_INFO("Sandbox2D Layer Attached");
	texture = Wildlands::Texture2D::Create("./assets/Textures/Checkerboard.png");
}

void Sandbox2D::Detach()
{
	WL_PROFILE_FUNCTION();
	WL_INFO("Sandbox2D Layer Detached");
}

void Sandbox2D::Update(Wildlands::Timestep ts)
{
	WL_PROFILE_FUNCTION();
	//Camera
	m_CameraController.OnUpdate(ts);

	{
		WL_PROFILE_SCOPE("Render");
		Wildlands::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Wildlands::RenderCommand::Clear();

		Wildlands::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Wildlands::Renderer2D::DrawQuad(position, size, color);
		Wildlands::Renderer2D::DrawQuad(position + glm::vec3{-0.8f, 0.8f, 0.0f}, size, {0.2, 0.2, 0.8, 1.0});

		glm::vec3 offset = { 1.8f, -0.5f, 0.0f };
		Wildlands::Renderer2D::DrawQuad(position + offset, size, texture);
		Wildlands::Renderer2D::DrawQuad({ -5.0f, -5.0f, -0.1f }, { 10.f, 10.f }, texture);

		Wildlands::Renderer2D::EndScene();
	}
}

void Sandbox2D::UIRender()
{
	WL_PROFILE_FUNCTION();
	ImGui::Begin("Settings");

	ImGui::SliderFloat3("Position", glm::value_ptr(position),-5.f, 5.f );
	ImGui::SliderFloat2("Size", glm::value_ptr(size),-5.f, 5.f );
	ImGui::ColorEdit4("Color", glm::value_ptr(color));

	ImGui::End();
}

void Sandbox2D::OnEvent(Wildlands::Event& event)
{
	m_CameraController.OnEvent(event);
}

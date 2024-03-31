#include "Sandbox2D.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}


void Sandbox2D::Attach()
{
	WL_PROFILE_FUNCTION();
	WL_INFO("Sandbox2D Layer Attached");
	m_Texture = Wildlands::Texture2D::Create("./assets/Textures/Checkerboard.png");

    Wildlands::FrameBufferSpecification framebufferSpec;
	framebufferSpec.Width = 1280;
    framebufferSpec.Height = 720;
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
	m_CameraController.Update(ts);

	Wildlands::Renderer2D::ResetStats();

	{
		WL_PROFILE_SCOPE("Render Pre");
		Wildlands::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Wildlands::RenderCommand::Clear();
	}

	{
		WL_PROFILE_SCOPE("Renderer Draw");

		Wildlands::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Wildlands::Renderer2D::DrawQuad(m_Position, m_Size, m_Color);
		Wildlands::Renderer2D::DrawQuad(m_Position, m_Size, m_Color);
		Wildlands::Renderer2D::DrawRotatedQuad(m_Position + glm::vec3{-0.8f, 0.8f, 0.0f}, m_Size, 45.f, {0.2, 0.2, 0.8, 1.0});

		glm::vec3 offset = { 1.8f, -0.5f, 0.0f };
		Wildlands::Renderer2D::DrawQuad(m_Position + offset, m_Size, m_Texture);
		Wildlands::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 30.f, 30.f }, m_Texture);

		//Wildlands::Renderer2D::EndScene();

		//Wildlands::Renderer2D::BeginScene(m_CameraController.GetCamera());
		
		/*float quadOffset = 0.1f;
		int quadNum = 40;
		float halfLength = ((quadNum - 1) * quadOffset + quadNum ) * 0.5f;
		for (float x = -halfLength; x < halfLength; x += (1 + quadOffset))
		{
			for (float y = -halfLength; y < halfLength; y += (1 + quadOffset))
			{
				glm::vec4 quadColor = { (x + halfLength) / (2 * halfLength), 0.4f, (y + halfLength) / (2 * halfLength), 0.6f };
				Wildlands::Renderer2D::DrawQuad({ x, y, 0.0f }, { 1.0f, 1.0f }, quadColor);
			}
		}*/
		Wildlands::Renderer2D::EndScene();
	}
}

void Sandbox2D::UIRender()
{
	WL_PROFILE_FUNCTION();
	ImGui::Begin("Settings");

	ImGui::SliderFloat3("Position", glm::value_ptr(m_Position),-5.f, 5.f );
	ImGui::SliderFloat2("Size", glm::value_ptr(m_Size),-5.f, 5.f );
	ImGui::ColorEdit4("Color", glm::value_ptr(m_Color));

	auto& stats = Wildlands::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.drawCalls);
	ImGui::Text("Quad Count: %d", stats.quadCount);
	ImGui::Text("Vertices: %d", stats.GetVertexCount());
	ImGui::Text("Indices: %d", stats.GetIndexCount());

	ImGui::End();//"settings"

}

void Sandbox2D::OnEvent(Wildlands::Event& event)
{
	m_CameraController.OnEvent(event);
}

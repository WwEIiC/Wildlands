#include "Sandbox2D.h"
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "Wildlands/Platforms/OpenGL/OpenGLShader.h"

#include <chrono>
template <typename Func>
class Timer
{
public:
	Timer(const char* name, Func&& func)
		: m_Name(name), m_Func(func), m_Stopped(false)
	{
		m_StartTimePoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped) { Stop(); }
	}

	void Stop()
	{
		auto EndTimePoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(EndTimePoint).time_since_epoch().count();

		float duration = (end - start) * 0.001f;
		m_Func({ m_Name, duration });

		m_Stopped = true;
	}

private:
	const char* m_Name;
	Func m_Func;
	bool m_Stopped;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimePoint;
};

#define PROFILE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); })

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}


void Sandbox2D::Attach()
{
	WL_INFO("Sandbox2D Layer Attached");
	texture = Wildlands::Texture2D::Create("./assets/Textures/Checkerboard.png");
}

void Sandbox2D::Detach()
{
	WL_INFO("Sandbox2D Layer Detached");
}

void Sandbox2D::Update(Wildlands::Timestep ts)
{
	PROFILE("Sandbox2D::Update");
	//Camera
	{
		PROFILE("CameraController::Update");
		m_CameraController.OnUpdate(ts);
	}

	Wildlands::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Wildlands::RenderCommand::Clear();

	Wildlands::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Wildlands::Renderer2D::DrawQuad(position, size, color);

	glm::vec3 offset = { 1.8f, -0.5f, 0.0f };
	Wildlands::Renderer2D::DrawQuad(position + offset, size, texture);
	Wildlands::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {10.f, 10.f}, texture);

	Wildlands::Renderer2D::EndScene();
}

void Sandbox2D::UIRender()
{
	ImGui::Begin("Settings");
	ImGui::SliderFloat3("Position", glm::value_ptr(position),-5.f, 5.f );
	ImGui::SliderFloat2("Size", glm::value_ptr(size),-5.f, 5.f );
	ImGui::ColorEdit4("Color", glm::value_ptr(color));

	for (auto& result : m_ProfileResults)
	{
		char label[50];
		strcpy(label, result.name);
		strcat(label, " %.3fms");
		ImGui::Text(label, result.time);
	}
	m_ProfileResults.clear();
	ImGui::End();

}

void Sandbox2D::OnEvent(Wildlands::Event& event)
{
	m_CameraController.OnEvent(event);
}

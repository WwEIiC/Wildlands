#include "WLPCH.h"
#include "CameraControllers.h"
#include "Wildlands/Core/Input.h"
#include "array"
#include <algorithm>
#include <vector>
namespace Wildlands
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio)
		: m_AspectRatio(aspectRatio), m_ZoomLevel(1), 
		m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel )
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		float speed = m_CameraMoveSpeed * m_ZoomLevel;
		glm::vec3 position = m_Camera.GetPosition();
		if (Input::IsKeyDown(WL_KEY_A)) { position.x -= speed * ts; }
		if (Input::IsKeyDown(WL_KEY_D)) { position.x += speed * ts; }
		if (Input::IsKeyDown(WL_KEY_W)) { position.y += speed * ts; }
		if (Input::IsKeyDown(WL_KEY_S)) { position.y -= speed * ts; }
		m_Camera.SetPosition(position);

		speed = m_CameraRotateSpeed * m_ZoomLevel;
		float rotation = m_Camera.GetRotation();
		if (Input::IsKeyDown(WL_KEY_Q)) { rotation += speed * ts; }
		if (Input::IsKeyDown(WL_KEY_E)) { rotation -= speed * ts; }
		m_Camera.SetRotation(rotation);
	}
	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FUNC(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(OrthographicCameraController::OnWindowResized));
	}
	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}

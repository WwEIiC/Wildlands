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
		WL_PROFILE_FUNCTION();

		float speed = m_CameraMoveSpeed * m_ZoomLevel;
		float rotation = m_Camera.GetRotation();

		glm::vec3 position = m_Camera.GetPosition();
		if (Input::IsKeyDown(WL_KEY_A)) 
		{ 
			position.x -= cos(glm::radians(rotation)) * speed * ts; 
			position.y -= sin(glm::radians(rotation)) * speed * ts;
		}
		if (Input::IsKeyDown(WL_KEY_D)) 
		{ 
			position.x += cos(glm::radians(rotation)) * speed * ts;
			position.y += sin(glm::radians(rotation)) * speed * ts;
		}
		if (Input::IsKeyDown(WL_KEY_W)) 
		{ 
			position.x += -sin(glm::radians(rotation)) * speed * ts;
			position.y += cos(glm::radians(rotation)) * speed * ts;
		}
		if (Input::IsKeyDown(WL_KEY_S)) 
		{ 
			position.x -= -sin(glm::radians(rotation)) * speed * ts;
			position.y -= cos(glm::radians(rotation)) * speed * ts;
		}
		m_Camera.SetPosition(position);

		speed = m_CameraRotateSpeed * m_ZoomLevel;
		if (Input::IsKeyDown(WL_KEY_Q)) { rotation += speed * ts; }
		if (Input::IsKeyDown(WL_KEY_E)) { rotation -= speed * ts; }
		if (rotation > 180.0f) { rotation -= 360.0f; }
		else if (rotation <= -180.0f) { rotation += 360.0f; }
		m_Camera.SetRotation(rotation);
	}
	void OrthographicCameraController::OnEvent(Event& e)
	{
		WL_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FUNC(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::ResizeViewport(float width, float height)
	{
		m_AspectRatio = width / height;
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		WL_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		WL_PROFILE_FUNCTION();
		
		ResizeViewport((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}
}

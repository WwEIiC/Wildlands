#pragma once

#include "Wildlands/Renderer/Cameras.h"
#include "Wildlands/Core/Timestep.h"

#include "Wildlands/Events/MouseEvent.h"
#include "Wildlands/Events/ApplicationEvent.h"

namespace Wildlands
{
	class OrthographicCameraController
	{
	public:
		/// <param name="aspectRatio">width / height</param>
		OrthographicCameraController(float aspectRatio);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		void ResizeViewport(float width, float height);
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel;
		float m_CameraMoveSpeed = 1.5f;
		float m_CameraRotateSpeed = 120.0f;

		OrthographicCamera m_Camera;
	};
}

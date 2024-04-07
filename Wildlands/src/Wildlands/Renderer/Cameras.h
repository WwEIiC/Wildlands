#pragma once
#include <glm/glm.hpp>

namespace Wildlands
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4 m_projectMatrix)
			: m_ProjectionMatrix(m_projectMatrix) {}
		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
	protected:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
	};

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetVPMatrix() const { return m_VPMatrix; }

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; UpdateViewMatrix(); }

		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; UpdateViewMatrix(); }

	private:
		void UpdateViewMatrix();
	private:
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_VPMatrix;
		glm::vec3 m_Position;
		float m_Rotation; //z axle rotation
	};


	class PerspectiveCamera
	{
	};
}


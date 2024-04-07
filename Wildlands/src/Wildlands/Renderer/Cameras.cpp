#include "WLPCH.h"
#include "Cameras.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Wildlands
{
	//////////////////////////////////// OrthographicCamera ///////////////////////////////////////////////
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ViewMatrix(1.0f), m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)),
		m_Position({0.0f, 0.0f, 0.0f}), m_Rotation(0.0f)
	{
		m_VPMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		WL_PROFILE_FUNCTION();

		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_VPMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::UpdateViewMatrix()
	{
		WL_PROFILE_FUNCTION();

		//transform is make a original vector to the camera.
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * 
							glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), { 0, 0, 1 });

		//view matrix is the inverse of the transform matrix. object move in opposite way to the camera.
		m_ViewMatrix = glm::inverse(transform);
		m_VPMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}

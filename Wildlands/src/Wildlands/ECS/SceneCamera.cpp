#include "WLPCH.h"
#include "SceneCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Wildlands
{
	SceneCamera::SceneCamera()
	{
		UpdataProjectionMatrix();
	}

	void SceneCamera::SetPerspective(float fov, float pNear, float pFar)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = fov;
		m_PerspectiveNear = pNear;
		m_PerspectiveFar = pFar;
		UpdataProjectionMatrix();
	}

	void SceneCamera::SetOrthographic(float oSize, float oNear, float oFar)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthoSize = oSize;
		m_OrthoNear = oNear;
		m_OrthoFar = oFar;
		UpdataProjectionMatrix();
	}
	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		WL_CORE_ASSERT(width > 0 && height > 0, "Viewport size must be greater than zero");

		m_AspectRatio = (float)width / (float)height;
		UpdataProjectionMatrix();
	}

	void SceneCamera::UpdataProjectionMatrix()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_ProjectionMatrix = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}

		else if (m_ProjectionType == ProjectionType::Orthographic)
		{
			float orthoLeft		= -m_OrthoSize * m_AspectRatio * 0.5f;
			float orthoRight	=  m_OrthoSize * m_AspectRatio * 0.5f;
			float orthoBottom	= -m_OrthoSize * 0.5f;
			float orthoTop		=  m_OrthoSize * 0.5f;

			m_ProjectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthoNear, m_OrthoFar);
		}
	}

}

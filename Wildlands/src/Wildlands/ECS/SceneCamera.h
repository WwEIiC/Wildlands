#pragma once
#include "Wildlands/Renderer/Cameras.h"

namespace Wildlands
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0, 
			Orthographic = 1
		};
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType (ProjectionType type) { m_ProjectionType = type; UpdataProjectionMatrix(); }

		////////////////////////////////////////// Perspective //////////////////////////////////////////////
		void SetPerspective(float fov, float pNear, float pFar);

		/// <returns> in radian </returns>
		float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		/// <summary>
		/// calculate in radian
		/// </summary>
		void SetPerspectiveFOV(float fov) { m_PerspectiveFOV = fov; UpdataProjectionMatrix();}

		float GetPerspectiveNear() const { return m_PerspectiveNear; }
		void SetPerspectiveNear(float nearClip) { m_PerspectiveNear = nearClip; UpdataProjectionMatrix();}

		float GetPerspectiveFar() const { return m_PerspectiveFar; }
		void SetPerspectiveFar(float farClip) { m_PerspectiveFar = farClip; UpdataProjectionMatrix();}

		
		///////////////////////////////////////// Orthographic /////////////////////////////////////////////
		void SetOrthographic(float oSize, float oNear, float oFar);
		void SetViewportSize(uint32_t width, uint32_t height);

		float GetOrthoSize() const { return m_OrthoSize; }
		void SetOrthoSize(float oSize) { m_OrthoSize = oSize; UpdataProjectionMatrix(); }

		float GetOrthoNearClip() const { return m_OrthoNear; }
		void SetOrthoNearClip(float nearClip) { m_OrthoNear = nearClip; UpdataProjectionMatrix(); }

		float GetOrthoFarClip() const { return m_OrthoFar; }
		void SetOrthoFarClip(float farClip) { m_OrthoFar = farClip; UpdataProjectionMatrix(); }


	private:
		void UpdataProjectionMatrix();
	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_AspectRatio = 1.0f;

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;

		float m_OrthoSize = 10.0f; // height
		float m_OrthoNear= -1.0f, m_OrthoFar= 1.0f;
	};

}


#pragma once
#include "Wildlands/Renderer/RenderCommand.h"
#include "Wildlands/Renderer/Cameras.h"
#include "Wildlands/Renderer/Shader.h"

namespace Wildlands
{

	class Renderer
	{
	public:
		inline static RendererAPI::EAPI GetAPI() { return RendererAPI::GetAPI(); }

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		//WLTODO:API change to const std::shared_ptr<Mesh>& when have class Mesh.
		static void Submit(const std::shared_ptr<Shader>& shader,
						const std::shared_ptr<VertexArray>& vertexArray,
						const glm::mat4& transform = glm::mat4(1.0f));

	private:
		static glm::mat4 s_VPMatrix;
	};
}


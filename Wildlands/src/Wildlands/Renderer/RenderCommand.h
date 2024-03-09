#pragma once
#include "Wildlands/Renderer/RendererAPI.h"

namespace Wildlands
{
	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color = glm::vec4(1.0f, 0.2f, 1.0f, 1.0f))
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndex(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndex(vertexArray);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};
}


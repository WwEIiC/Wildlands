#pragma once
#include "Wildlands/Renderer/RendererAPI.h"

namespace Wildlands
{
	class RenderCommand
	{
	public:
		inline static void Init() { s_RendererAPI->Init(); }

		inline static void SetClearColor(const glm::vec4& color = glm::vec4(1.0f, 0.2f, 1.0f, 1.0f))
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndex(const Ref<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndex(vertexArray);
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};
}


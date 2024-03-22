#pragma once
#include "Wildlands/Renderer/RendererAPI.h"

namespace Wildlands
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual ~OpenGLRendererAPI() = default;

		virtual void Init() override;

		virtual void SetClearColor(const glm::vec4& color = glm::vec4(1.0f, 0.2f, 1.0f, 1.0f)) override;
		virtual void Clear() override;
		virtual void DrawIndex(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	};
}


#pragma once
#include "Wildlands/Renderer/RendererAPI.h"

namespace Wildlands
{
	class OpenGLRendererAPI : public RendererAPI
	{
		virtual ~OpenGLRendererAPI() = default;

		virtual void Init() override;

		virtual void SetClearColor(const glm::vec4& color = glm::vec4(1.0f, 0.2f, 1.0f, 1.0f)) override;
		virtual void Clear() override;
		virtual void DrawIndex(const Ref<VertexArray>& vertexArray) override;
	};
}


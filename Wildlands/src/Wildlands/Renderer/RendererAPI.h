#pragma once

#include "glm/glm.hpp"
#include "Wildlands/Renderer/VertexArray.h"

namespace Wildlands
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};

	public:
		static Unique<RendererAPI> Create();
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		inline static API GetAPI() { return s_API; }
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void SetClearColor(const glm::vec4& color = glm::vec4(1.0f, 0.2f, 1.0f, 1.0f)) = 0;
		virtual void Clear() = 0;
		virtual void DrawIndex(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void DrawLine(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
		virtual void SetLineWidth(float width) = 0;
	private:
		static API s_API;
	};

}


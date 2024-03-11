#pragma once
#include "Wildlands/Renderer/Buffers.h"
#include <memory>

namespace Wildlands
{
	class VertexArray
	{
	public:
		static Ref<VertexArray> Create();
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;
	};
}

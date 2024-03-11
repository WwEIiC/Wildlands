#pragma once

#include "Wildlands/Renderer/VertexArray.h"

namespace Wildlands
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray() override;

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; };
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; };

	private:
		uint32_t m_RendererID;
		uint32_t m_VertexAttributeIndex = 0;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}


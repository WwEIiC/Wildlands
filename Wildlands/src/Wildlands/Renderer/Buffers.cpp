#include "WLPCH.h"
#include "Buffers.h"

#include "Wildlands/Renderer/Renderer.h"
#include "Wildlands/Platforms/OpenGL/OpenGLBuffers.h"

namespace Wildlands
{
	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
		: m_Elements(elements), m_Stride(0)
	{
		CalculateOffsetAndStride();
	}

	
	void BufferLayout::CalculateOffsetAndStride()
	{
		uint32_t offset = 0;
		m_Stride = 0;

		for (auto& element : m_Elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}


	VertexBuffer* VertexBuffer::Create(float* vectices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::EAPI::None:
			WL_CORE_ASSERT(false, "RendererAPI is None");
			return nullptr;

		case RendererAPI::EAPI::OpenGL:
			return new OpenGLVertexBuffer(vectices, size);
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::EAPI::None:
			WL_CORE_ASSERT(false, "RendererAPI is None");
			return nullptr;

		case RendererAPI::EAPI::OpenGL:
			return new OpenGLIndexBuffer(indices, size);
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}

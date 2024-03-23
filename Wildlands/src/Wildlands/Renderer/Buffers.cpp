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
		m_Stride = 0;

		for (auto& element : m_Elements)
		{
			element.Offset = m_Stride;
			m_Stride += element.Size;
		}
	}


	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::EAPI::None:
			WL_CORE_ASSERT(false, "RendererAPI is None");
			return nullptr;

		case RendererAPI::EAPI::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(size);
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vectices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::EAPI::None:
			WL_CORE_ASSERT(false, "RendererAPI is None");
			return nullptr;

		case RendererAPI::EAPI::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(vectices, size);
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::EAPI::None:
			WL_CORE_ASSERT(false, "RendererAPI is None");
			return nullptr;

		case RendererAPI::EAPI::OpenGL:
			return CreateRef<OpenGLIndexBuffer>(indices, count);
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}

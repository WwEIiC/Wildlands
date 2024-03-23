#include "WLPCH.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Wildlands
{
	static GLenum EShaderDataTypeToOpenGLType(EShaderDataType type)
	{
		switch (type)
		{
		case EShaderDataType::Float:		return GL_FLOAT;
		case EShaderDataType::Float2:		return GL_FLOAT;
		case EShaderDataType::Float3:		return GL_FLOAT;
		case EShaderDataType::Float4:		return GL_FLOAT;
		case EShaderDataType::Mat3:			return GL_FLOAT;
		case EShaderDataType::Mat4:			return GL_FLOAT;
		case EShaderDataType::Int:			return GL_INT;
		case EShaderDataType::Int2:			return GL_INT;
		case EShaderDataType::Int3:			return GL_INT;
		case EShaderDataType::Int4:			return GL_INT;
		case EShaderDataType::Bool:			return GL_BOOL;
		}

		WL_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		WL_PROFILE_FUNCTION();

		glGenVertexArrays(1, &m_RendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		WL_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		WL_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}
	void OpenGLVertexArray::UnBind() const
	{
		WL_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	/// <summary>
	/// bind the layout inside the vertex buffer
	/// </summary>
	/// <param name="vertexBuffer"></param>
	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		WL_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		WL_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer layout is Empty");

		const auto& layout = vertexBuffer->GetLayout();
		//WLFIXME::when element.type == mat3/4 the offset maybe wrong.
		for (const auto& element : layout)
		{
			glVertexAttribPointer(m_VertexAttributeIndex, element.GetCount(), EShaderDataTypeToOpenGLType(element.Type),
				element.Normalized, layout.GetStride(), (void*)(intptr_t)element.Offset);
			glEnableVertexAttribArray(m_VertexAttributeIndex++);
		}

		glBindVertexArray(0);
		m_VertexBuffers.push_back(vertexBuffer);
	}
	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		WL_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();
		glBindVertexArray(0);

		m_IndexBuffer = indexBuffer;
	}
}

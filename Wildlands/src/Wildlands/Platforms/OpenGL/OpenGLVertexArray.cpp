#include "WLPCH.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Wildlands
{
	static GLenum EShaderDataTypeToOpenGLType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:		return GL_FLOAT;
		case ShaderDataType::Float2:		return GL_FLOAT;
		case ShaderDataType::Float3:		return GL_FLOAT;
		case ShaderDataType::Float4:		return GL_FLOAT;
		case ShaderDataType::Mat3:			return GL_FLOAT;
		case ShaderDataType::Mat4:			return GL_FLOAT;
		case ShaderDataType::Int:			return GL_INT;
		case ShaderDataType::Int2:			return GL_INT;
		case ShaderDataType::Int3:			return GL_INT;
		case ShaderDataType::Int4:			return GL_INT;
		case ShaderDataType::UInt:			return GL_UNSIGNED_INT;
		case ShaderDataType::Bool:			return GL_BOOL;
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
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				glVertexAttribPointer(m_VertexAttributeIndex, element.GetCount(), EShaderDataTypeToOpenGLType(element.Type),
				element.Normalized, layout.GetStride(), (void*)(intptr_t)element.Offset);
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::UInt:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glVertexAttribIPointer(m_VertexAttributeIndex,
					element.GetCount(),
					EShaderDataTypeToOpenGLType(element.Type),
					layout.GetStride(),
					(const void*)(intptr_t)element.Offset);
				break;
			}

			default:
				WL_CORE_ASSERT(false, "Unknown Shader Data type");
				break;
			}
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

#pragma once
#include "Wildlands/Core/Base.h"
//Render API abstract

namespace Wildlands
{
	enum class ShaderDataType
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4, UInt,
		Mat3, Mat4,
		Bool
	};

	static uint32_t SizeOfShaderDataType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::UInt:		return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Bool:     return 1;
		}

		WL_CORE_ASSERT(false, "Unknown Type of ShaderData");
		return 0;
	}

	struct BufferElement
	{
		ShaderDataType Type;
		std::string Name;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement() = default;
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Type(type), Name(name), Size(SizeOfShaderDataType(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetCount() const
		{
			switch (Type)
			{
			case Wildlands::ShaderDataType::None:		return 0;
			case Wildlands::ShaderDataType::Float:		return 1;
			case Wildlands::ShaderDataType::Float2:	return 2;
			case Wildlands::ShaderDataType::Float3:	return 3;
			case Wildlands::ShaderDataType::Float4:	return 4;
			case Wildlands::ShaderDataType::Int:		return 1;
			case Wildlands::ShaderDataType::UInt:		return 1;
			case Wildlands::ShaderDataType::Int2:		return 2;
			case Wildlands::ShaderDataType::Int3:		return 3;
			case Wildlands::ShaderDataType::Int4:		return 4;
			case Wildlands::ShaderDataType::Mat3:		return 3 * 3;
			case Wildlands::ShaderDataType::Mat4:		return 4 * 4;
			case Wildlands::ShaderDataType::Bool:		return 1;
			}

			WL_CORE_ASSERT(false, "Unknown Type of ShaderData");
			return -1;
		}

	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements);

		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		inline const uint32_t GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetAndStride();

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride;
	};

	class VertexBuffer
	{
	public:
		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(float* vectices, uint32_t size);
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;
	};

	class IndexBuffer
	{
	public:
		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual uint32_t GetCount() const = 0;
	};
}


#pragma once
#include "Wildlands/Core/Base.h"

namespace Wildlands
{
	enum class FrameBufferTextureFormat
	{
		None = 0,
		// Color
		RGBA8,
		RGBA16F,
		RGBA32F,

		// Depth/Stencil
		Depth24Stencil8,
		Depth32FStencil8,

		//Defaults
		Depth = Depth24Stencil8
	};
	struct FrameBufferTextureSpecification
	{
		FrameBufferTextureSpecification() = default;
		FrameBufferTextureSpecification(FrameBufferTextureFormat format)
			: TextureFormat(format)
		{
		}

		FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
		//WLTODO:: Filter and Wrap
	};

	// specificate all the attachments that the framebuffer will have
	struct FrameBufferAttachmentSpecification
	{
		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments)
			: Attachments(attachments)
		{
		}

		std::vector<FrameBufferTextureSpecification> Attachments;
	};
	struct FrameBufferSpecification
	{
		FrameBufferSpecification() = default;
		uint32_t Width, Height;
		uint32_t Samples = 1;// > 1 means multisample.

		FrameBufferAttachmentSpecification Attachments;
	};

	class FrameBuffer
	{
	public:
		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
		virtual ~FrameBuffer() = default;

		virtual const FrameBufferSpecification& GetSpecification() const = 0;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void Resize(uint32_t widht, uint32_t height) = 0;
	};
}

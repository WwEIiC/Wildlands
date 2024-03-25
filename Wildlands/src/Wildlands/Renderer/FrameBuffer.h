#pragma once
#include "Wildlands/Core/Base.h"

namespace Wildlands
{
	struct FrameBufferSpecification
	{
		uint32_t Width, Height;
		//bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
		virtual ~FrameBuffer() = default;

		virtual const FrameBufferSpecification& GetSpecification() const = 0;
		virtual uint32_t GetColorAttachmentRendererID() const = 0;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void Resize(uint32_t widht, uint32_t height) = 0;
	};
}

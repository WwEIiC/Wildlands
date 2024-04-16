#pragma once
#include "Wildlands/Renderer/FrameBuffer.h"

namespace Wildlands
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer() override;

		virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index) const override 
		{ 
			WL_CORE_ASSERT(index < m_ColorAttachments.size(), "It hasn't has much attachment.")
			return m_ColorAttachments[index]; 
		}

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual int ReadPixelAsInt(uint32_t attachmentIndex, int x, int y) override;
		virtual uint32_t ReadPixelAsUInt(uint32_t attachmentIndex, int x, int y) override;

		virtual void ClearColorAttachment(uint32_t attachmentIndex, const void* value) override;

	private:
		void NewFrameBuffer();
	private:
		uint32_t m_RendererID;
		FrameBufferSpecification m_Specification;

		std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecifications;
		FrameBufferTextureSpecification m_DepthAttachmentSpecification;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}

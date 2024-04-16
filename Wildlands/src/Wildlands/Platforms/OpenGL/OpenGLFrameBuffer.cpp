#include "WLPCH.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Wildlands
{
	namespace Utils
	{
		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}
		static void CreateTextures(bool multisampled, uint32_t count, uint32_t* outID)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t tid)
		{
			glBindTexture(TextureTarget(multisampled), tid);
		}

		static void AttachColorTexture(uint32_t tid, int samples,
			GLenum storageFormat, GLenum accessFormat,
			uint32_t width, uint32_t height, int attachIndex)
		{
			bool multisampled = samples > 1;
			if (multisampled)
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, storageFormat, width, height, GL_FALSE);
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, storageFormat, width, height, 0, accessFormat, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachIndex, TextureTarget(multisampled), tid, 0);
		}
		static void AttachDepthTexture(uint32_t tid, int samples, 
			GLenum fomat, GLenum attachmentType,
			uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, fomat, width, height, GL_FALSE);
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, fomat, width, height);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), tid, 0);
		}

		static bool IsDepthFormat(FrameBufferTextureFormat format)
		{
			switch (format)
			{
			case FrameBufferTextureFormat::Depth24Stencil8:
			case FrameBufferTextureFormat::Depth32FStencil8:
				return true;
			}
			return false;
		}

		static GLenum WLFBTextureFormatToGL(FrameBufferTextureFormat WLFormat)
		{
			switch (WLFormat)
			{
			case FrameBufferTextureFormat::None:
				return GL_NONE;
			case FrameBufferTextureFormat::RGBA8:
				return GL_RGBA8;
			case FrameBufferTextureFormat::RGBA16F:
				return GL_RGBA16F;
			case FrameBufferTextureFormat::RGBA32F:
				return GL_RGBA32F;
			case FrameBufferTextureFormat::RED_INTEGER:
				return GL_RED_INTEGER;
			case FrameBufferTextureFormat::RED_UINTEGER:
				return GL_RED_INTEGER;
			case FrameBufferTextureFormat::Depth24Stencil8:
				return GL_DEPTH24_STENCIL8;
			case FrameBufferTextureFormat::Depth32FStencil8:
				return GL_DEPTH32F_STENCIL8;
			default:
				return GL_FLOAT;
			}

		}
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
		: m_RendererID(0), m_Specification(spec)
	{
		for (auto spec: m_Specification.Attachments.Attachments)
		{
			if (Utils::IsDepthFormat(spec.TextureFormat))
				m_DepthAttachmentSpecification = spec;
			else
				m_ColorAttachmentSpecifications.emplace_back(spec);
		}
		NewFrameBuffer();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}


	void OpenGLFrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFrameBuffer::UnBind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		static uint32_t MaxFrameBufferSize = 1024 * 8;
		if (width == 0 || height == 0 || width > MaxFrameBufferSize || height > MaxFrameBufferSize)
		{
			WL_CORE_WARN("Attempted to resize framebuffer to ({0}, {1})", width, height);
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		NewFrameBuffer();
	}

	int OpenGLFrameBuffer::ReadPixelAsInt(uint32_t attachmentIndex, int x, int y)
	{
		WL_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "attachmentIndex out of range");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	uint32_t OpenGLFrameBuffer::ReadPixelAsUInt(uint32_t attachmentIndex, int x, int y)
	{
		WL_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "attachmentIndex out of range");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		uint32_t pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFrameBuffer::ClearColorAttachment(uint32_t attachmentIndex, const void* value)
	{
		WL_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "attachmentIndex out of range");

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];

		// the type of value.
		// now it is assumed that, for one FBFormat it only provides one type of value.
		GLenum dataType = GL_FLOAT;
		switch (spec.TextureFormat)
		{
		case FrameBufferTextureFormat::RED_INTEGER:
			dataType = GL_INT;
			break;
		case FrameBufferTextureFormat::RED_UINTEGER:
			dataType = GL_UNSIGNED_INT;;
			break;
		default:
			break;
		}

		glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
			Utils::WLFBTextureFormatToGL(spec.TextureFormat), dataType, value);
	}

	void OpenGLFrameBuffer::NewFrameBuffer()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;
		// Attachments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			Utils::CreateTextures(multisample, m_ColorAttachments.size(), m_ColorAttachments.data());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				Utils::BindTexture(multisample, m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
				case FrameBufferTextureFormat::RGBA8:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples,
						GL_RGBA8, GL_RGBA,
						m_Specification.Width, m_Specification.Height, i);
					break;
				case FrameBufferTextureFormat::RED_INTEGER:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples,
						GL_R32I, GL_RED_INTEGER,
						m_Specification.Width, m_Specification.Height, i);

					break;
				case FrameBufferTextureFormat::RED_UINTEGER:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples,
						GL_R32UI, GL_RED_INTEGER,
						m_Specification.Width, m_Specification.Height, i);

					break;
				default:
					break;
				}
			}
		}
		if (m_DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
		{
			Utils::CreateTextures(multisample, 1, &m_DepthAttachment);
			Utils::BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
			case FrameBufferTextureFormat::Depth24Stencil8:
				Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8,
					GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			default:
				break;
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			WL_CORE_ASSERT(m_ColorAttachments.size() <= 4, "only use four attachment now");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// only depth-pass.
			glDrawBuffer(GL_NONE);
		}

		WL_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer isn't complete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

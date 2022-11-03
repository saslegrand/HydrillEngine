#include "Renderer/Primitives/FrameBuffer.hpp"

#include <glad/gl.h>

#include "Core/Logger.hpp"

FrameBuffer::FrameBuffer(InternalFormat internalTextureFormat, Format textureFormat)
	: m_textureInternalFormat(static_cast<int>(internalTextureFormat)), m_textureFormat(static_cast<int>(textureFormat))
{
	m_textureFormat = static_cast<int>(textureFormat);

	glGenFramebuffers(1, &m_ID);
	glGenTextures(1, &m_multiSampleTextureBuffer);
	glGenRenderbuffers(1, &m_captureRenderBuffer);
	glGenFramebuffers(1, &m_intermediateCaptureFrameBuffer);
	glGenTextures(1, &m_resultTextureBuffer);
}

FrameBuffer::FrameBuffer(FrameBuffer&& cpy) noexcept
{
	m_ID = cpy.m_ID;
	m_multiSampleTextureBuffer = cpy.m_multiSampleTextureBuffer;
	m_captureRenderBuffer = cpy.m_captureRenderBuffer;
	m_intermediateCaptureFrameBuffer = cpy.m_intermediateCaptureFrameBuffer;
	m_resultTextureBuffer = cpy.m_resultTextureBuffer;

	m_resized = cpy.m_resized;
	m_dimensions = cpy.m_dimensions;
	m_previousFB = cpy.m_previousFB;
	m_textureFormat = cpy.m_textureFormat;
	m_textureInternalFormat = cpy.m_textureInternalFormat;

	cpy.m_ID = 0;
	cpy.m_multiSampleTextureBuffer = 0;
	cpy.m_captureRenderBuffer = 0;
	cpy.m_intermediateCaptureFrameBuffer = 0;
	cpy.m_resultTextureBuffer = 0;
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& cpy) noexcept
{
	this->~FrameBuffer();

	m_ID = cpy.m_ID;
	m_multiSampleTextureBuffer = cpy.m_multiSampleTextureBuffer;
	m_captureRenderBuffer = cpy.m_captureRenderBuffer;
	m_intermediateCaptureFrameBuffer = cpy.m_intermediateCaptureFrameBuffer;
	m_resultTextureBuffer = cpy.m_resultTextureBuffer;

	m_resized = cpy.m_resized;
	m_dimensions = cpy.m_dimensions;
	m_previousFB = cpy.m_previousFB;
	m_textureFormat = cpy.m_textureFormat;
	m_textureInternalFormat = cpy.m_textureInternalFormat;

	cpy.m_ID = 0;
	cpy.m_multiSampleTextureBuffer = 0;
	cpy.m_captureRenderBuffer = 0;
	cpy.m_intermediateCaptureFrameBuffer = 0;
	cpy.m_resultTextureBuffer = 0;

	return *this;
}

FrameBuffer::~FrameBuffer()
{
	glDeleteTextures(1, &m_resultTextureBuffer);
	glDeleteTextures(1, &m_multiSampleTextureBuffer);
	glDeleteFramebuffers(1, &m_ID);
	glDeleteFramebuffers(1, &m_intermediateCaptureFrameBuffer);
	glDeleteRenderbuffers(1, &m_captureRenderBuffer);
}

void FrameBuffer::AttachNewTexture(unsigned int textureID)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateCaptureFrameBuffer);

	//	Generate finale texture
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, m_textureInternalFormat, static_cast<GLsizei>(m_dimensions.x), static_cast<GLsizei>(m_dimensions.y), 0, m_textureFormat, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
	}

	//	Then check if the frame buffer is complete
	GLenum intermediateFramebufferStatus = glCheckNamedFramebufferStatus(m_intermediateCaptureFrameBuffer, GL_FRAMEBUFFER);
	if (intermediateFramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::Error("FrameBuffer - Uncomplete framebuffer");
	}

	//	Then check if the frame buffer is complete
	GLenum Test = glCheckNamedFramebufferStatus(m_ID, GL_FRAMEBUFFER);
	if (Test != GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::Error("FrameBuffer - Uncomplete framebuffer");
	}
}

void FrameBuffer::Create()
{
	// Save current bound framebuffer
	GLint previousFramebuffer;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousFramebuffer);

	//	Generate and bind the FrameBufferObject
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

	//	Generate a multisample texture to render into then bind it to the FrameBufferObject
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_multiSampleTextureBuffer);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, m_textureInternalFormat, static_cast<GLsizei>(m_dimensions.x), static_cast<GLsizei>(m_dimensions.y), GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 0, GL_TEXTURE_2D_MULTISAMPLE, m_multiSampleTextureBuffer, 0);
	}

	//	Generate a render buffer, we need it for depth & stencil testing
	{
		glBindRenderbuffer(GL_RENDERBUFFER, m_captureRenderBuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(m_dimensions.x), static_cast<GLsizei>(m_dimensions.y));
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_captureRenderBuffer);
	}

	//	Then check if the frame buffer is complete
	GLenum framebufferStatus = glCheckNamedFramebufferStatus(m_ID, GL_FRAMEBUFFER);
	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::Error("FrameBuffer - Uncomplete framebuffer");
	}

	//	Intermediate frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateCaptureFrameBuffer);

	//	Generate finale texture
	{
		glBindTexture(GL_TEXTURE_2D, m_resultTextureBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, m_textureInternalFormat, static_cast<GLsizei>(m_dimensions.x), static_cast<GLsizei>(m_dimensions.y), 0, m_textureFormat, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_resultTextureBuffer, 0);
	}

	//	Then check if the frame buffer is complete
	GLenum intermediateFramebufferStatus = glCheckNamedFramebufferStatus(m_intermediateCaptureFrameBuffer, GL_FRAMEBUFFER);
	if (intermediateFramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::Error("FrameBuffer - Uncomplete framebuffer");
	}

	//	Rebind saved framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
}

void  FrameBuffer::Update()
{
	if (m_resized)
	{
		ApplySize();
		m_resized = false;
	}
}

void FrameBuffer::ApplySize()
{
	//	Apply new size to the frame buffer

	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRenderBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(m_dimensions.x), static_cast<GLsizei>(m_dimensions.y));
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_multiSampleTextureBuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, m_textureInternalFormat, static_cast<GLsizei>(m_dimensions.x), static_cast<GLsizei>(m_dimensions.y), GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glBindTexture(GL_TEXTURE_2D, m_resultTextureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, m_textureInternalFormat, static_cast<GLsizei>(m_dimensions.x), static_cast<GLsizei>(m_dimensions.y), 0, m_textureFormat, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

float FrameBuffer::GetWidth() const
{
	return m_dimensions.x;
}

float FrameBuffer::GetHeight() const
{
	return m_dimensions.y;
}

Vector2 FrameBuffer::GetDimensions() const
{
	return m_dimensions;
}

void  FrameBuffer::SetDimensions(const float width, const float height)
{
	m_dimensions.x = width;
	m_dimensions.y = height;

	m_resized = true;
}

void  FrameBuffer::SetDimensions(const Vector2 dimensions)
{
	m_dimensions = dimensions;

	m_resized = true;
}

void FrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
}

void FrameBuffer::EndDraw() const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_intermediateCaptureFrameBuffer);
	glBlitFramebuffer(0, 0, static_cast<GLint>(m_dimensions.x), static_cast<GLint>(m_dimensions.y), 0, 0, static_cast<GLint>(m_dimensions.x), static_cast<GLint>(m_dimensions.y), GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void FrameBuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int FrameBuffer::GetImage() const
{
	return m_resultTextureBuffer;
}

bool FrameBuffer::WasResized() const
{
	return m_resized;
}

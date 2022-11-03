#include "Renderer/Primitives/RenderBuffer.hpp"

#include <glad/gl.h>

RenderBuffer::RenderBuffer()
{
	glCreateBuffers(1, &m_ID);
}

RenderBuffer::RenderBuffer(RenderBuffer&& cpy) noexcept
{
	m_ID = cpy.m_ID;

	cpy.m_ID = 0;
}

RenderBuffer& RenderBuffer::operator=(RenderBuffer&& cpy) noexcept
{
	this->~RenderBuffer();

	m_ID = cpy.m_ID;
	cpy.m_ID = 0;

	return *this;
}

RenderBuffer::~RenderBuffer()
{
	glDeleteBuffers(1, &m_ID);
}

void RenderBuffer::Bind() const
{
	glBindBuffer(GL_RENDERBUFFER, m_ID);
}

void RenderBuffer::Unbind() const
{
	glBindBuffer(GL_RENDERBUFFER, 0);
}
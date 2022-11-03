#include "Renderer/Primitives/VertexBuffer.hpp"

#include <glad/gl.h>

VertexBuffer::VertexBuffer()
{
	glCreateBuffers(1, &m_ID);
}

VertexBuffer::VertexBuffer(VertexBuffer&& cpy) noexcept
{
	m_ID = cpy.m_ID;
	size = cpy.size;

	cpy.m_ID = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& cpy) noexcept
{
	this->~VertexBuffer();

	m_ID = cpy.m_ID;
	size = cpy.size;

	cpy.m_ID = 0;

	return *this;
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_ID);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
#include "Renderer/Primitives/VertexArray.hpp"

#include <glad/gl.h>

VertexArray::VertexArray()
{
	glCreateVertexArrays(1, &m_ID);
}

VertexArray::VertexArray(VertexArray&& cpy) noexcept
{
	m_ID = cpy.m_ID;
	cpy.m_ID = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& cpy) noexcept
{
	this->~VertexArray();

	m_ID = cpy.m_ID;
	cpy.m_ID = 0;

	return *this;
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_ID);
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_ID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}
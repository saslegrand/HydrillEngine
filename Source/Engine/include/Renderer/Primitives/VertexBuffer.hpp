#pragma once

#include "Renderer/Primitives/GLPrimitive.hpp"

struct ENGINE_API VertexBuffer : public GLPrimitive
{
	unsigned int size = 0;

	VertexBuffer();
	VertexBuffer(const VertexBuffer& cpy) = delete;
	VertexBuffer(VertexBuffer&& cpy) noexcept;

	VertexBuffer& operator=(const VertexBuffer& cpy) = delete;
	VertexBuffer& operator=(VertexBuffer&& cpy) noexcept;

	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};
#pragma once

#include "Renderer/Primitives/GLPrimitive.hpp"

struct ENGINE_API VertexArray : public GLPrimitive
{
private:

public:
	VertexArray();
	VertexArray(const VertexArray& cpy) = delete;
	VertexArray(VertexArray&& cpy) noexcept;

	VertexArray& operator=(const VertexArray & cpy) = delete;
	VertexArray& operator=(VertexArray && cpy) noexcept;

	~VertexArray();

	void Bind() const;
	void Unbind() const;
};
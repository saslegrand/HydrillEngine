#pragma once

#include "Renderer/Primitives/GLPrimitive.hpp"

struct ENGINE_API RenderBuffer : public GLPrimitive
{
	RenderBuffer();
	RenderBuffer(const RenderBuffer& cpy) = delete;
	RenderBuffer(RenderBuffer&& cpy) noexcept;

	RenderBuffer& operator=(const RenderBuffer& cpy) = delete;
	RenderBuffer& operator=(RenderBuffer&& cpy) noexcept;

	~RenderBuffer();

	void Bind() const;
	void Unbind() const;
};
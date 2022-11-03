#pragma once

#include "Renderer/Primitives/GLPrimitive.hpp"

class Texture;

struct ENGINE_API GLTexture : public GLPrimitive
{
	GLTexture();

	GLTexture(const GLTexture& cpy) = delete;
	GLTexture(GLTexture&& cpy) noexcept;

	GLTexture& operator=(const GLTexture& cpy) = delete;
	GLTexture& operator=(GLTexture&& cpy) noexcept;

	~GLTexture();

	void Bind() const;
	void BindUnit(int unit) const;
	void Unbind() const;
	void UnbindUnit(int unit) const;

	void Generate(const Texture& texture);
};
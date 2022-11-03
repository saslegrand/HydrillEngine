#pragma once

#include <queue>

#include "Renderer/Primitives/GLPrimitive.hpp"

class Texture;

struct TextureArrayConfig
{
	unsigned int target   = 0x8C1A;
	unsigned int maxSize  = 100;
	unsigned int width    = 512;
	unsigned int height   = 512;
	unsigned int internalFormat = 0x8051;
	unsigned int format    = 0x1907;
	unsigned int type	   = 0x1401;
	unsigned int magFilter = 0x2601;
	unsigned int minFilter = 0x2601;
	unsigned int wrap	   = 0x2901;
	
	float borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};


struct ENGINE_API GLTextureArray : public GLPrimitive
{
	GLTextureArray(TextureArrayConfig config = TextureArrayConfig());
	GLTextureArray(const GLTextureArray& cpy) = delete;
	GLTextureArray(GLTextureArray&& cpy) noexcept;

	GLTextureArray& operator=(const GLTextureArray& cpy) = delete;
	GLTextureArray& operator=(GLTextureArray&& cpy) noexcept;

	~GLTextureArray();

	// Disable std warning
#pragma warning(disable:4251)
	std::queue<int> freeLayers;
#pragma warning(default:4251)

	TextureArrayConfig config;

	void Bind() const;
	void BindUnit(int unit) const;
	void Unbind() const;
	void UnbindUnit(int unit) const;

	void Generate();
	void Add(Texture& texture);
	void Remove(const Texture& texture);
};
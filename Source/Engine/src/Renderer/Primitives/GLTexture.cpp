#include "Renderer/Primitives/GLTexture.hpp"

#include <glad/gl.h>

#include "Resources/Resource/Texture.hpp"

GLTexture::GLTexture()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
}

GLTexture::GLTexture(GLTexture&& cpy) noexcept
{
	m_ID = cpy.m_ID;

	cpy.m_ID = 0;
}

GLTexture& GLTexture::operator=(GLTexture&& cpy) noexcept
{
	this->~GLTexture();

	m_ID = cpy.m_ID;
	cpy.m_ID = 0;

	return *this;
}

GLTexture::~GLTexture()
{
	glDeleteTextures(1, &m_ID);
}

void GLTexture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void GLTexture::BindUnit(int unit) const
{
	glBindTextureUnit(unit, m_ID);
}

void GLTexture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture::UnbindUnit(int unit) const
{
	glBindTextureUnit(unit, 0);
}

void GLTexture::Generate(const Texture& texture)
{
	if (texture.data.data == nullptr)
	{
		return;
	}

	Bind();

	GLint texParamWrap = texture.GetLoadingFlags().TestBit(EImageSTB::IMG_WRAP_REPEAT) ? GL_REPEAT : GL_CLAMP_TO_EDGE;

	bool hasMipmaps = texture.GetLoadingFlags().TestBit(EImageSTB::IMG_GEN_MIPMAPS);
	GLint texParamFilter = hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texParamWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texParamWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texParamFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, texture.data.format, texture.data.width, texture.data.height, 0, texture.data.format, texture.data.type, texture.data.data);

	if (hasMipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	Unbind();
}
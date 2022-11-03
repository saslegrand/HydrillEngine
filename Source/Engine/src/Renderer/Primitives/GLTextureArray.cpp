#include "Renderer/Primitives/GLTextureArray.hpp"

#include <glad/gl.h>

#include "Core/Logger.hpp"
#include "Resources/Resource/Texture.hpp"
#include "Resources/Parsers/AssimpParser.hpp"


GLTextureArray::GLTextureArray(TextureArrayConfig config)
	:config(config)
{
	glCreateTextures(config.target, 1, &m_ID);

	for (size_t i = 0; i < config.maxSize; i++)
	{
		freeLayers.push(static_cast<int>(i));
	}
}

GLTextureArray::GLTextureArray(GLTextureArray&& cpy) noexcept
{
	m_ID = cpy.m_ID;
	config = cpy.config;

	cpy.m_ID = 0;
}

GLTextureArray& GLTextureArray::operator=(GLTextureArray&& cpy) noexcept
{
	this->~GLTextureArray();

	m_ID = cpy.m_ID;
	config = cpy.config;

	cpy.m_ID = 0;

	return *this;
}

GLTextureArray::~GLTextureArray()
{
	glDeleteTextures(1, &m_ID);
}

void GLTextureArray::Bind() const
{
	glBindTexture(config.target, m_ID);
}

void GLTextureArray::BindUnit(int unit) const
{
	glBindTextureUnit(unit, m_ID);
}

void GLTextureArray::Unbind() const
{
	glBindTexture(config.target, 0);
}

void GLTextureArray::UnbindUnit(int unit) const
{
	glBindTextureUnit(unit, m_ID);
}


void GLTextureArray::Generate()
{
	Bind();

	glTexParameteri(config.target, GL_TEXTURE_MIN_FILTER, config.minFilter);
	glTexParameteri(config.target, GL_TEXTURE_MAG_FILTER, config.minFilter);
	glTexParameteri(config.target, GL_TEXTURE_WRAP_S, config.wrap);
	glTexParameteri(config.target, GL_TEXTURE_WRAP_T, config.wrap);
	glTexParameteri(config.target, GL_TEXTURE_WRAP_R, config.wrap);
	glTexParameterfv(config.target, GL_TEXTURE_BORDER_COLOR, config.borderColor);

	if (config.format == GL_DEPTH_COMPONENT)
	{
		glTexParameteri(config.target, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(config.target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	}

	glTexImage3D(config.target, 0, config.internalFormat, config.width, config.height, config.maxSize, 0, config.format, config.type, nullptr);

	Unbind();
}

void GLTextureArray::Add(Texture& texture)
{
	if (freeLayers.empty())
	{
		Logger::Warning("GLTextureArray - Add - Maximum layer reached in texture array");
		return;
	}

	if (texture.GPUData->registeredTextureArray)
	{
		return;
	}

	Bind();

	GLTexture* srctexture = texture.GPUData->generatedTexture;
	
	if (texture.data.data == nullptr)
	{
		Flags<EImageSTB> flag = texture.GetLoadingFlags();
		flag.Disable(EImageSTB::IMG_FORCE_GREY, EImageSTB::IMG_FORCE_GREY_ALPHA, EImageSTB::IMG_FORCE_RGBA);
		flag.Enable(EImageSTB::IMG_FORCE_RGB);
		AssimpParser::LoadTexture(texture, flag, false);
	}

	if (config.target == GL_TEXTURE_2D_ARRAY)
	{
		glTexSubImage3D(config.target,
			0,
			0, 0, freeLayers.front(),
			Maths::Min(static_cast<int>(config.width), texture.data.width),
			Maths::Min(static_cast<int>(config.height), texture.data.height),
			1,
			config.format,
			config.type,
			texture.data.data
		);
	}
	else if (config.target == GL_TEXTURE_CUBE_MAP_ARRAY)
	{
		//	Unsupported yet
		//	Maybe do come texture target check check 
	}

	Unbind();

	texture.GPUData->registeredTextureArray = this;
	texture.GPUData->arrayIndex = freeLayers.front();

	freeLayers.pop();
}


void GLTextureArray::Remove(const Texture& texture)
{
	glTexSubImage3D(config.target,
		0,
		0, 0, texture.GPUData->arrayIndex,
		Maths::Min(static_cast<int>(config.width), texture.data.width),
		Maths::Min(static_cast<int>(config.height), texture.data.height),
		1,
		config.format,
		config.type,
		nullptr
	);

	freeLayers.push(texture.GPUData->arrayIndex);

	texture.GPUData->registeredTextureArray = nullptr;
	texture.GPUData->arrayIndex = -1;
}
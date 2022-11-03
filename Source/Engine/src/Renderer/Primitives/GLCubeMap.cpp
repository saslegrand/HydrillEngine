#include "Renderer/Primitives/GLCubeMap.hpp"

#include <glad/gl.h>

#include "Resources/Resource/Skybox.hpp"
#include "Resources/Resource/Texture.hpp"
#include "Resources/Parsers/AssimpParser.hpp"

GLCubeMap::GLCubeMap()
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_ID);
}

GLCubeMap::GLCubeMap(GLCubeMap&& cpy) noexcept
{
	m_ID = cpy.m_ID;

	cpy.m_ID = 0;
}

GLCubeMap& GLCubeMap::operator=(GLCubeMap&& cpy) noexcept
{
	this->~GLCubeMap();

	m_ID = cpy.m_ID;
	cpy.m_ID = 0;

	return *this;
}

GLCubeMap::~GLCubeMap()
{
	glDeleteTextures(1, &m_ID);
}

void GLCubeMap::Bind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
}

void GLCubeMap::BindUnit(int unit) const
{
	glBindTextureUnit(unit, m_ID);
}

void GLCubeMap::Unbind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GLCubeMap::UnbindUnit(int unit) const
{
	glBindTextureUnit(unit, 0);
}

void GLCubeMap::Generate(Skybox& skybox)
{
	Bind();

	if (skybox.right == nullptr)
	{
		Logger::Warning("Skybox - Face 0 is NULL, discard generation");
		return;
	}

	Texture** textures = &skybox.right;
	TextureData const& texData = textures[0]->data;

	int width = texData.width;
	int height = texData.height;
	int type = texData.type;

	// Check faces validity
	for (uint32_t i = 0; i < 6; i++)
	{
		Texture* face = textures[i];

		if (face == nullptr)
		{
			Logger::Warning("Skybox - Face " + std::to_string(i) + " is NULL, discard generation");
			return;
		}

		if (!face->GetLoadingFlags().TestBit(EImageSTB::IMG_FORCE_RGB))
		{
			Logger::Warning("Skybox - Face " + std::to_string(i) + " is missing the ForceRGB flag, generation discard");
			return;
		}

		// Faces need to have same type and same dimensions
		if (face->data.width	!= width  ||
			face->data.height	!= height ||
			face->data.type		!= type)
		{
			Logger::Warning("Skybox - Texture dimensions or types are not the same, discard generation");
			return;
		}
	}

	// Load and generate skybox faces
	for (uint32_t i = 0; i < 6; i++)
	{
		Texture* face = textures[i];

		// Create temp texture to load data
		Texture tex = Texture(HYGUID("0"));
		tex.originalPath = face->originalPath;

		// Load texture
		if (!AssimpParser::LoadTexture(tex, face->GetLoadingFlags(), false))
			continue;

		// Set skybox data
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, type, tex.data.data);

		// Free texture
		AssimpParser::FreeTexture(tex);
	}

	// Set cube map texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	Unbind();
}

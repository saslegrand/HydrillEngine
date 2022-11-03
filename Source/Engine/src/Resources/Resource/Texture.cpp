#include <fstream>
#include <glad/gl.h>
#include <stb_image/stb_image.h>
#include <stb_image/stb_image_write.h>

#include "EngineContext.hpp"
#include "Resources/ResourcesManager.hpp"
#include "Resources/Loaders/ResourcesLoader.hpp"
#include "Resources/Parsers/ParserFlags.hpp"
#include "Tools/StringHelper.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Tools/SerializationUtils.hpp"

#include "Generated/Texture.rfks.h"

rfk::UniquePtr<Texture> Texture::defaultInstantiator(const HYGUID& uid)
{
	return rfk::makeUnique<Texture>(uid);
}

Texture::Texture(const HYGUID& uid)
	: Resource(uid, staticGetArchetype().getName())
{
	m_type = RESOURCE_TYPE::TEXTURE;

	m_flag.Enable(EImageSTB::IMG_FLIP, EImageSTB::IMG_GEN_MIPMAPS, EImageSTB::IMG_WRAP_REPEAT);
}

Flags<EImageSTB> const& Texture::GetLoadingFlags() const
{
	return m_flag;
}

bool Texture::Import(const std::string& path)
{
	originalPath = path;
	Resource::Import(StringHelper::GetFileNameWithoutExtension(path) + ".texture");

	return ResourcesLoader::LoadTextureUnsafe(*this);
}

void Texture::LoadInGPUMemory()
{
	SystemManager::GetRenderSystem().GPUWrapper.CreateTextureData(*this);
	ResourcesLoader::FreeTextureData(*this);
}

void Texture::UnloadFromGPUMemory()
{
	SystemManager::GetRenderSystem().GPUWrapper.RemoveTextureData(*this);
}

void Texture::Serialize()
{
	json j;

	j["extension"] = StringHelper::GetFileExtensionFromPath(originalPath);

	// Save textures uIds
	j["flags"] = m_flag.GetFlagAsInt();

	WriteJson(m_filepath, j);
}

void Texture::Deserialize()
{
	json j;
	ReadJson(m_filepath, j);

	std::string ext;
	Serialization::TryGetValue(j, "extension", ext);
	originalPath = StringHelper::GetFilePathWithoutExtension(m_filepath) + ext;

	uint64_t flags = 0;
	if (Serialization::TryGetValue(j, "flags", flags))
	{
		m_flag.SetFlagFromInt(flags);
	}

	ResourcesLoader::LoadTextureUnsafe(*this);
}

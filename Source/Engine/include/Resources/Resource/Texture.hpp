#pragma once

#include <vector>

#include "Resources/Resource/Resource.hpp"
#include "Resources/Parsers/ParserFlags.hpp"
#include "Renderer/GPUTextureData.hpp"
#include "Tools/Flags.hpp"

#include "Generated/Texture.rfkh.h"

struct GLTexture;

struct TextureData
{
	int width = 0;
	int height = 0;
	int channels = 0;
	int internalFormat = 0;
	int format = 0;
	int type = 0;
	unsigned char* data = nullptr;
};

/**
@brief Texture 
*/
class HY_CLASS() Texture : public Resource
{
private:
	HY_METHOD(rfk::Instantiator)
		static rfk::UniquePtr<Texture> defaultInstantiator(const HYGUID & uid);

	HY_FIELD() Flags<EImageSTB> m_flag;

public:
	std::string originalPath;
	TextureData data;
	GPUTextureData*  GPUData;

	Texture(const HYGUID& uid);

	Flags<EImageSTB> const& GetLoadingFlags() const;

	virtual bool Import(const std::string& path) override;
	virtual void LoadInGPUMemory() override;
	virtual void UnloadFromGPUMemory() override;

	virtual void Serialize() override;
	virtual void Deserialize() override;

	Texture_GENERATED
};

File_Texture_GENERATED

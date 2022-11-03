#pragma once

#include <array>

#include "Resources/Resource/Resource.hpp"
#include "EngineDLL.hpp"

#include "Generated/Skybox.rfkh.h"

struct GLCubeMap;
class Texture;

/**
@brief Skybox 
*/
class HY_CLASS() Skybox : public Resource
{
private:
	HY_METHOD(rfk::Instantiator)
		static rfk::UniquePtr<Skybox> defaultInstantiator(const HYGUID & uid);

public:
	HY_FIELD() Texture* right = nullptr;
	HY_FIELD() Texture* left = nullptr;
	HY_FIELD() Texture* up = nullptr;
	HY_FIELD() Texture* down = nullptr;
	HY_FIELD() Texture* front = nullptr;
	HY_FIELD() Texture* back = nullptr;

	GLCubeMap* GPUData;

	ENGINE_API Skybox(const HYGUID& uid);

	ENGINE_API void Reload();
	virtual void LoadInGPUMemory() override;
	virtual void UnloadFromGPUMemory() override;

	virtual void Serialize() override;
	virtual void Deserialize() override;

	Skybox_GENERATED
};

File_Skybox_GENERATED

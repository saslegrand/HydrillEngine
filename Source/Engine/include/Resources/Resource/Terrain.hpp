#pragma once


#include "Resources/Resource/Resource.hpp"

#include "Generated/Terrain.rfkh.h"

namespace physx { class PxHeightField; class PxHeightFieldDesc; }
using namespace physx;

class Texture;
class Mesh;

/**
@brief Terrains contains the height map texture, for generating and rendering a height mesh and its collider
*/
class HY_CLASS() Terrain : public Resource
{
private:
	HY_METHOD(rfk::Instantiator)
		static rfk::UniquePtr<Terrain> defaultInstantiator(const HYGUID & uid);

public:
	Mesh* heightMapMesh = nullptr;
	HY_FIELD() Texture* heightMapTexture = nullptr;

	HY_FIELD() int LOD = 1;
	HY_FIELD() int rowNb = 0;
	HY_FIELD() int columnNb = 0;
	HY_FIELD() float rowScale = 1.0f;
	HY_FIELD() float columnScale = 1.0f;
	HY_FIELD() float heightScale = 1.0f;

	ENGINE_API Terrain(const HYGUID& uid);

	ENGINE_API void Reload(bool forceReload);

	virtual void LoadInGPUMemory() override;
	virtual void UnloadFromGPUMemory() override {}

	virtual void Serialize() override;
	virtual void Deserialize() override;

	Terrain_GENERATED
};

File_Terrain_GENERATED

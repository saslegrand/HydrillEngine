#include <fstream>

#include <nlohmann/json.hpp>

#include <physx/geometry/PxHeightField.h>

#include "Resources/Resource/Texture.hpp"
#include "Resources/Resource/Mesh.hpp"
#include "EngineContext.hpp"
#include "Resources/ResourcesManager.hpp"
#include "Resources/Loaders/ResourcesLoader.hpp"
#include "Tools/SerializationUtils.hpp"

#include "Generated/Terrain.rfks.h"

rfk::UniquePtr<Terrain> Terrain::defaultInstantiator(const HYGUID& uid)
{
	return rfk::makeUnique<Terrain>(uid);
}

Terrain::Terrain(const HYGUID& uid)
	: Resource(uid, staticGetArchetype().getName())
{
	m_type = RESOURCE_TYPE::TERRAIN;
}

void Terrain::LoadInGPUMemory()
{
	Reload(false);
}

void Terrain::Reload(bool forceReload)
{
	ResourcesLoader::LoadTerrain(*this, forceReload);
}

void Terrain::Serialize()
{
	json j;
	
	j["heightMapTexture"] = heightMapTexture ? std::string(heightMapTexture->GetUID()) : "0";
	j["heightMapMesh"] = heightMapMesh ? std::string(heightMapMesh->GetUID()) : "0";
	j["LOD"] = LOD;
	j["rowScale"] = rowScale;
	j["columnScale"] = columnScale;
	j["heightScale"] = heightScale;
	j["rowNb"] = rowNb;
	j["columnNb"] = columnNb;

	WriteJson(m_filepath, j);
}


void Terrain::Deserialize()
{
	json j;
	ReadJson(m_filepath, j);
	
	Serialization::TryGetResource(j, "heightMapTexture", heightMapTexture);
	Serialization::TryGetResource(j, "heightMapMesh", heightMapMesh);

	Serialization::TryGetValue(j, "LOD", LOD);

	Serialization::TryGetValue(j, "rowScale", rowScale);
	Serialization::TryGetValue(j, "columnScale", columnScale);
	Serialization::TryGetValue(j, "heightScale", heightScale);

	Serialization::TryGetValue(j, "rowNb", rowNb);
	Serialization::TryGetValue(j, "columnNb", columnNb);
}

#include <fstream>

#include <nlohmann/json.hpp>
#include "Resources/Resource/Texture.hpp"

#include "EngineContext.hpp"
#include "Resources/ResourcesManager.hpp"
#include "Tools/SerializationUtils.hpp"
#include "Tools/PathConfig.hpp"
#include "Maths/Vector2.hpp"

#include "Generated/ShaderType.rfks.h"
#include "Generated/MaterialSurface.rfks.h"
#include "Generated/Material.rfks.h"

rfk::UniquePtr<Material> Material::defaultInstantiator(const HYGUID& uid)
{
	return rfk::makeUnique<Material>(uid);
}

Material::Material(const HYGUID& uid)
	: Resource(uid, staticGetArchetype().getName())
{
	m_type = RESOURCE_TYPE::MATERIAL;
}

void Material::Serialize()
{
	json j;
	
	Serialization::SetContainer<float>(j["diffuseColor"],  data.diffuseColor);
	Serialization::SetContainer<float>(j["ambientColor"],  data.ambientColor);
	Serialization::SetContainer<float>(j["specularColor"], data.specularColor);
	Serialization::SetContainer<float>(j["emissiveColor"], data.emissiveColor);
	Serialization::SetContainer<float>(j["tiling"],	data.tiling);
	Serialization::SetContainer<float>(j["offset"],	data.offset);

	j["surface"]	  = static_cast<int>(surface);
	j["shader"]		  = static_cast<int>(shader);

	j["emissiveStrength"] = data.emissiveStrength;
	j["metalness"]		  = data.metalness;
	j["roughness"]		  = data.roughness;
	j["normalFactor"]	  = data.normalFactor;
	j["aoFactor"]		  = data.aoFactor;

	j["ambientTexture"]		= data.ambientTexture ? std::string(data.ambientTexture->GetUID()) : "0";
	j["diffuseTexture"]		= data.diffuseTexture ? std::string(data.diffuseTexture->GetUID()) : "0";
	j["specularTexture"]	= data.specularTexture ? std::string(data.specularTexture->GetUID()) : "0";
	j["emissiveTexture"]	= data.emissiveTexture ? std::string(data.emissiveTexture->GetUID()) : "0";
	j["metallicTexture"]	= data.metallicTexture ? std::string(data.metallicTexture->GetUID()) : "0";
	j["roughnessTexture"]	= data.roughnessTexture ? std::string(data.roughnessTexture->GetUID()) : "0";
	j["normalTexture"]		= data.normalTexture ? std::string(data.normalTexture->GetUID()) : "0";
	j["aoTexture"]			= data.aoTexture ? std::string(data.aoTexture->GetUID()) : "0";
	j["maskTexture"]		= data.aoTexture ? std::string(data.aoTexture->GetUID()) : "0";

	WriteJson(m_filepath, j);
}


void Material::Deserialize()
{
	json j;
	ReadJson(m_filepath, j);

	Serialization::TryGetContainer<float>(j, "diffuseColor", data.diffuseColor);
	Serialization::TryGetContainer<float>(j, "ambientColor", data.ambientColor);
	Serialization::TryGetContainer<float>(j, "specularColor", data.specularColor);
	Serialization::TryGetContainer<float>(j, "emissiveColor", data.emissiveColor);
	Serialization::TryGetContainer<float>(j, "tiling", data.tiling);
	Serialization::TryGetContainer<float>(j, "offset", data.offset);

	Serialization::TryGetValue(j, "surface", surface);
	Serialization::TryGetValue(j, "shader", shader);

	Serialization::TryGetValue(j, "emissiveStrength", data.emissiveStrength);
	Serialization::TryGetValue(j, "metalness", data.metalness);
	Serialization::TryGetValue(j, "roughness", data.roughness);
	Serialization::TryGetValue(j, "normalFactor", data.normalFactor);
	Serialization::TryGetValue(j, "aoFactor", data.aoFactor);
	
	Serialization::TryGetResource(j, "ambientTexture", data.ambientTexture);
	Serialization::TryGetResource(j, "diffuseTexture", data.diffuseTexture);
	Serialization::TryGetResource(j, "specularTexture", data.specularTexture);
	Serialization::TryGetResource(j, "emissiveTexture", data.emissiveTexture);
	Serialization::TryGetResource(j, "metallicTexture", data.metallicTexture);
	Serialization::TryGetResource(j, "roughnessTexture", data.roughnessTexture);
	Serialization::TryGetResource(j, "normalTexture", data.normalTexture);
	Serialization::TryGetResource(j, "aoTexture", data.aoTexture);
	Serialization::TryGetResource(j, "maskTexture", data.maskTexture);
}

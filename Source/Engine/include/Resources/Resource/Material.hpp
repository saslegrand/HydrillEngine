#pragma once

#include "Resources/Resource/Resource.hpp"
#include "Types/Color.hpp"
#include "Maths/Vector2.hpp"
#include "Renderer/MaterialSurface.hpp"
#include "Renderer/ShaderType.hpp"

#include "Generated/Material.rfkh.h"

class Texture;

struct HY_STRUCT() MaterialData : public BaseObject
{
	HY_FIELD(PHeader("UV parameters"))
			   alignas(8) Vector2 offset = Vector2::Zero;
	HY_FIELD() alignas(8) Vector2 tiling = Vector2::One;

	HY_FIELD(PHeader("Default Colors")) 
			   alignas(16) Color4 diffuseColor  = Color4::White;
	HY_FIELD() alignas(16) Color3 ambientColor  = Color3::Black;
	HY_FIELD() alignas(16) Color3 specularColor = Color3::White;
	HY_FIELD() alignas(16) Color3 emissiveColor = Color3::Black;

	HY_FIELD(PHeader("Default values"),PRange(0.0f, 100.f))
			   alignas(4) float emissiveStrength = 1.0f;

	HY_FIELD(PHeader("PBR values"), PRange(0.0f,1.f))
								 alignas(4)  float metalness	 = 0.0f;
	HY_FIELD(PRange(0.0f, 1.f))  alignas(4)  float roughness	 = 1.0f;
	HY_FIELD(PRange(0.0f, 5.f))  alignas(4) float normalFactor = 1.0f;
	HY_FIELD(PRange(0.0f, 1.f))  alignas(4) float aoFactor	 = 1.0f;

	HY_FIELD(PHeader("Default Textures")) 
			   Texture* diffuseTexture		= nullptr;
	HY_FIELD(PReadOnly()) Texture* ambientTexture = nullptr; // UNUSED
	HY_FIELD() Texture* normalTexture		= nullptr;
	HY_FIELD() Texture* emissiveTexture		= nullptr;
	HY_FIELD() Texture* maskTexture			= nullptr;

	HY_FIELD(PHeader("Lit Textures")) 
			   Texture* specularTexture		= nullptr;

	HY_FIELD(PHeader("PBR Textures")) 
			   Texture* metallicTexture		= nullptr;
	HY_FIELD() Texture* roughnessTexture	= nullptr;
	HY_FIELD() Texture* aoTexture			= nullptr;

	MaterialData_GENERATED
};

/**
@brief Material contains all textures, colors, and parameters, for rendering a mesh
*/
class HY_CLASS() Material : public Resource
{
private:
	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<Material> defaultInstantiator(const HYGUID & uid);

public:

	HY_FIELD(PHeader("Material base"))
			   MaterialSurface surface = MaterialSurface::OPAQUE;
	HY_FIELD() ShaderType shader = ShaderType::PBR;
	HY_FIELD() MaterialData data;

	ENGINE_API Material(const HYGUID& uid);

	virtual void LoadInGPUMemory() override {}
	virtual void UnloadFromGPUMemory() override {}

	virtual void Serialize() override;
	virtual void Deserialize() override;

	Material_GENERATED
};

File_Material_GENERATED

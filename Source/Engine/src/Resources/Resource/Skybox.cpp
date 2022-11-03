#include <fstream>
#include <glad/gl.h>
#include <stb_image/stb_image.h>
#include <stb_image/stb_image_write.h>

#include "EngineContext.hpp"

#include "Resources/ResourcesManager.hpp"
#include "Resources/Resource/Texture.hpp"
#include "Resources/Loaders/ResourcesLoader.hpp"
#include "Renderer/RenderSystem.hpp"

#include "Generated/Skybox.rfks.h"


rfk::UniquePtr<Skybox> Skybox::defaultInstantiator(const HYGUID& uid)
{
	return rfk::makeUnique<Skybox>(uid);
}

Skybox::Skybox(const HYGUID& uid)
	: Resource(uid, staticGetArchetype().getName())
{
	m_type = RESOURCE_TYPE::SKYBOX;
}

void Skybox::LoadInGPUMemory()
{
	SystemManager::GetRenderSystem().GPUWrapper.CreateSkyboxData(*this);
}

void Skybox::UnloadFromGPUMemory()
{
	SystemManager::GetRenderSystem().GPUWrapper.RemoveSkyboxData(*this);
}

void Skybox::Reload()
{
	// Reserialize to save changes
	Serialize();

	if (!GPUData)
	{
		LoadInGPUMemory();
		return;
	}

	// Regenerate to update skybox texture
	GPUData->Generate(*this);
}

void Skybox::Serialize()
{
	json j;

	// Save textures uIds
	j["textures"]	= { front	? std::string(front->GetUID())	: std::string("0"),
						back	? std::string(back->GetUID())	: std::string("0"),
						left	? std::string(left->GetUID())	: std::string("0"),
						right	? std::string(right->GetUID())	: std::string("0"),
						up		? std::string(up->GetUID())		: std::string("0"),
						down	? std::string(down->GetUID())	: std::string("0") };

	WriteJson(m_filepath, j);
}

void Skybox::Deserialize()
{
	json j;
	ReadJson(m_filepath, j);

	auto it = j.find("textures");

	if (it != j.end())
	{
		std::vector<std::string> textureIds = it.value();

		ResourcesManager* RM = EngineContext::Instance().resourcesManager;

		front = RM->GetResource<Texture>(HYGUID(textureIds[0]));
		back = RM->GetResource<Texture>(HYGUID(textureIds[1]));
		left = RM->GetResource<Texture>(HYGUID(textureIds[2]));
		right = RM->GetResource<Texture>(HYGUID(textureIds[3]));
		up = RM->GetResource<Texture>(HYGUID(textureIds[4]));
		down = RM->GetResource<Texture>(HYGUID(textureIds[5]));
	}
}

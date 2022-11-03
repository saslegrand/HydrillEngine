#include "Resources/ResourcesManager.hpp"

#include "Resources/Loaders/ResourcesLoader.hpp"

#include "Resources/Resource/Shader.hpp"
#include "Resources/Resource/Texture.hpp"
#include "Resources/Resource/Mesh.hpp"
#include "Resources/Resource/Model.hpp"
#include "Resources/Resource/Material.hpp"
#include "Resources/Resource/Prefab.hpp"
#include "Resources/Resource/Sound.hpp"
#include "Renderer/MeshShader.hpp"

#include "Tools/PathConfig.hpp"
#include "Tools/StringHelper.hpp"

#include "Core/Logger.hpp"



ResourcesManager::ResourcesManager()
{
}

ResourcesManager::~ResourcesManager()
{
}

void ResourcesManager::Initialize()
{
	ResourcesLoader::LoadResourcesInDirectory(INTERNAL_ENGINE_RESOURCES_ROOT);
	ResourcesLoader::LoadResourcesInDirectory(ASSETS_ROOT);

	MeshShader::SetDefaultMaterial(&static_cast<Material*>(GetResourceByPath(INTERNAL_ENGINE_RESOURCES_ROOT + std::string(R"(Objects\DefaultMaterial.mat)")))->data);
}

void ResourcesManager::CreateResourceFiles(Resource* resource)
{
	ResourcesLoader::CreateResourceFiles(resource);
}

Resource* ResourcesManager::CreateResource(const std::string& typeString, const HYGUID& id)
{
	// Check if the class is reflected and is a resource
	rfk::Class const* c = rfk::getDatabase().getFileLevelClassByName(typeString.c_str());
	if (!c || !c->isSubclassOf(Resource::staticGetArchetype()))
	{
		Logger::Error("ResourcesManager - " + typeString + " doesnt exist or is not based of Resource");
		return nullptr;
	}

	// Create the new resource
	m_resources.emplace(id, c->makeUniqueInstance<Resource>(id));

	return m_resources[id].get();
}

void ResourcesManager::DeleteResource(const HYGUID& uid)
{
	if (!HasResource(uid))
		return;

	std::lock_guard resGuard(m_resourceMutex);

	// Delete files
	ResourcesLoader::DeleteResourceFiles(m_resources[uid].get());

	// Delete GPU memory (and remove from systems)
	m_resources[uid]->UnloadFromGPUMemory();

	// Destroy the resource
	m_resources.erase(uid);
}

void ResourcesManager::DeleteResource(Resource* resource)
{
	if (resource == nullptr)
		return;

	DeleteResource(resource->GetUID());
}

bool ResourcesManager::HasResource(const HYGUID& uid)
{
	std::lock_guard resGuard(m_resourceMutex);
	return m_resources.find(uid) != m_resources.end();
}

bool ResourcesManager::HasResourceByPath(const std::string& path)
{
	std::lock_guard resGuard(m_resourceMutex);

	for (auto& it : m_resources)
	{
		if (it.second->GetFilepath() == path)
			return true;
	}
	return false;
}

bool ResourcesManager::HasResourceByFilename(const std::string& name)
{
	std::lock_guard resGuard(m_resourceMutex);

	for (auto& it : m_resources)
	{
		if (it.second->GetFilename() == name)
			return true;
	}
	return false;
}


Resource* ResourcesManager::GetResource(const HYGUID& uid)
{
	if (HasResource(uid))
		return m_resources[uid].get();

	return nullptr;
}

std::unordered_map<HYGUID, std::unique_ptr<Resource>>& ResourcesManager::GetResources()
{
	return m_resources;
}

Resource* ResourcesManager::GetResourceByPath(const std::string& path)
{
	std::lock_guard resGuard(m_resourceMutex);

	for (auto& it : m_resources)
	{
		if (it.second->GetFilepath() == path)
			return it.second.get();
	}

	return nullptr;
}

Resource* ResourcesManager::GetResourceByFilename(const std::string& name)
{
	std::lock_guard resGuard(m_resourceMutex);

	for (auto& it : m_resources)
	{
		std::string f = it.second->GetFilename();
		if (f == name)
			return it.second.get();
	}

	return nullptr;
}

std::vector<Resource*> ResourcesManager::GetResourcesList(RESOURCE_TYPE type)
{
	std::vector<Resource*> resourcesList;
	for (auto& it : m_resources)
	{
		if (it.second->GetType() == type)
			resourcesList.emplace_back(it.second.get());
	}
	return resourcesList;
}

std::vector<std::string> ResourcesManager::GetResourcesNamesList(RESOURCE_TYPE type)
{
	std::vector<std::string> nameList;
	for (auto& it : m_resources)
	{
		if (it.second->GetType() == type)
			nameList.emplace_back(it.second->GetFilename());
	}
	return nameList;
}

ResourcesTaskPool& ResourcesManager::GetTaskPool()
{
	return m_taskPool;
}

void ResourcesManager::ImportNewResource(const std::string& path)
{
	std::string ext = StringHelper::GetFileExtensionFromPath(path);
	ExtensionType extType = ResourcesLoader::GetExtensionType(ext);

	switch (extType)
	{
	case ExtensionType::MODEL:
		ImportResource<Model>(path);
		return;
	case ExtensionType::TEXTURE:
		ImportResource<Texture>(path);
		return;
	case ExtensionType::SOUND:
		ImportResource<Sound>(path);
		return;
	default:
		Logger::Warning("ResourcesManager : Failed to load new resource, file extension not supported of file " + std::string(path));
	}
}

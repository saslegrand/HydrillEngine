
inline void ResourcesManager::DestroyResource(const HYGUID& uid)
{
	std::lock_guard resGuard(m_resourceMutex);

	// Erase the resource
	m_resources.erase(uid);
}

template <DerivedResource TResource>
TResource* ResourcesManager::CreateResource()
{
	std::lock_guard queueGuard(m_resourceMutex);

	HYGUID id = HYGUID::NewGUID();
	m_resources.emplace(id, std::make_unique<TResource>(id));

	return static_cast<TResource*>(m_resources[id].get());
}

template <DerivedResource TResource>
TResource* ResourcesManager::CreateResource(const std::string& path)
{
	TResource* resource = CreateResource<TResource>();
	resource->SetFileInfo(path);
	CreateResourceFiles(resource);
	
	return resource;
}

template <DerivedResource TResource>
TResource* ResourcesManager::GetResource(const HYGUID& id)
{
	return static_cast<TResource*>(GetResource(id));
}

template <DerivedResource TResource>
void ResourcesManager::ImportResource(const std::string& path)
{
	// Create empty resource from type
	TResource* newResource = CreateResource<TResource>();

	if (newResource->Import(path))
	{
		CreateResourceFiles(newResource);

		m_taskPool.AddSingleThreadTask([newResource]() {
			newResource->LoadInGPUMemory();
			});
		return;
	}

	Logger::Error("ResourcesManager - Failed to load '" + path + "'");

	// Erase the resource if not valid
	DestroyResource(newResource->GetUID());
}
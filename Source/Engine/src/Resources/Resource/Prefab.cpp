#include <fstream>

#include "ECS/GameObject.hpp"

#include "Resources/Loaders/ResourcesLoader.hpp"

#include "EngineContext.hpp"

#include "Generated/Prefab.rfks.h"

rfk::UniquePtr<Prefab> Prefab::defaultInstantiator(const HYGUID& uid)
{
	return rfk::makeUnique<Prefab>(uid);
}

Prefab::Prefab(const HYGUID& uid)
	: Resource(uid, staticGetArchetype().getName())
{
	m_type = RESOURCE_TYPE::PREFAB;
}

Prefab::Prefab(const HYGUID& uid, const std::string& name)
	: Resource(uid, name)
{
	m_type = RESOURCE_TYPE::PREFAB;
}

void Prefab::CreateFromGameObject(GameObject* gameObject, const std::string& name, const std::string& path)
{
	if (name == "")
		SetFilename(gameObject->GetName() + ".prefab");
	else
		SetFilename(name + ".prefab");

	SetFilepath(path + "\\" + GetFilename());

	ResourcesLoader::SavePrefab(*this, *gameObject);
	ResourcesLoader::CreateResourceFiles(this);
}

json& Prefab::GetJsonGraph()
{
	return m_jsonGraph;
}

void Prefab::Serialize()
{
	WriteJson(m_filepath, m_jsonGraph);
}

void Prefab::Deserialize()
{
	ReadJson(m_filepath, m_jsonGraph);
}
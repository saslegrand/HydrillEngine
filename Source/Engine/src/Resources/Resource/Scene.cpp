#include <fstream>
#include <nlohmann/json.hpp>

#include "ECS/Behavior.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Tools/SerializationUtils.hpp"
#include "Resources/Loaders/ResourcesLoader.hpp"
#include "Resources/ResourcesManager.hpp"
#include "Resources/Resource/Prefab.hpp"
#include "Resources/Resource/Skybox.hpp"

#include "EngineContext.hpp"
#include "Resources/SceneManager.hpp"

#include "Generated/Scene.rfks.h"

rfk::UniquePtr<Scene> Scene::defaultInstantiator(const HYGUID& uid)
{
	return rfk::makeUnique<Scene>(uid);
}

Scene::Scene(const HYGUID& uid)
	: Resource(uid, staticGetArchetype().getName()), m_root("SCENE_ROOT")
{
	m_type = RESOURCE_TYPE::SCENE;
	EngineContext::Instance().sceneManager->Register(this);
}

Scene::~Scene()
{
	SceneManager* sceneManager = EngineContext::Instance().sceneManager;
	sceneManager->Unregister(this);
}

GameObject* Scene::Instantiate(Prefab* prefab)
{
	return nullptr;
}

GameObject* Scene::Instantiate(Prefab* prefab, GameObject* parent, Vector3 position, Quaternion rotation, Vector3 scale)
{
	if (!prefab)
		return nullptr;

	GameObject* newGameObject = ResourcesLoader::LoadPrefab(*prefab, *this);
	newGameObject->SetParent(parent);
	newGameObject->transform.SetLocalPosition(position);
	newGameObject->transform.SetLocalRotation(rotation);
	newGameObject->transform.SetLocalScale(scale);
	return newGameObject;
}

GameObject* Scene::Instantiate(const std::string& name, GameObject* parent, Vector3 position, Quaternion rotation, Vector3 scale)
{
	GameObject* newGameObject = CreateGameObject(name);
	newGameObject->SetParent(parent);
	newGameObject->transform.SetPosition(position);
	newGameObject->transform.SetRotation(rotation);
	newGameObject->transform.SetScale(scale);
	return newGameObject;
}

void Scene::DestroyGameObject(GameObject& GO, bool shouldCallOnDestroy)
{
	auto it = m_gameObjects.find(GO.GetUID());
	if (it != m_gameObjects.end())
	{
		GameObject& GO = *it->second.get();

		// Destroy children
		while (GO.GetChildCount() > 0)
			DestroyGameObject(*GO.GetChild(0), shouldCallOnDestroy);

		// Call OnDestroy (Remove and call OnDestroy for components)
		GO.OnDestroy(shouldCallOnDestroy);

		// Destroy GO
		m_gameObjects.erase(it);
		return;
	}

	// Should not happen
	Logger::Error("Scene - GameObject '" + GO.GetName() + "' is not registered in the current scene");
	return;
}

GameObject& Scene::GetRoot()
{
	return m_root;
}

json& Scene::GetJsonGraph()
{
	return m_jsonGraph;
}

GameObject& Scene::DuplicateGameObject(GameObject& GO)
{
	Prefab copyPrefab{ HYGUID("") };
	ResourcesLoader::SavePrefab(copyPrefab, GO);

	Transform& transform = GO.transform;
	GameObject* copyGO = SceneManager::Instantiate(&copyPrefab, GO.GetParent(), transform.LocalPosition(), transform.LocalRotation(), transform.LocalScale());

	copyGO->SetName(GO.GetName() + "_copy");

	return *copyGO;
}

GameObject* Scene::GetGameObject(const HYGUID& uid)
{
	if (m_gameObjects.find(uid) != m_gameObjects.end())
		return m_gameObjects[uid].get();
	return nullptr;
}

GameObject* Scene::GetGameObjectFromName(const HyString& name)
{
	for (auto& [key, value] : m_gameObjects)
	{
		if (value->GetName().compare(name.data()) == 0)
			return value.get();
	}

	Logger::Warning("Scene - GameObject with name '" + std::string(name.data()) + "' doesnt exist");
	return nullptr;
}

GameObject* Scene::CreateGameObject(const std::string& name, const HYGUID& uid)
{
	GameObject* go = (m_gameObjects[uid] = std::make_unique<GameObject>(name, uid)).get();
	go->SetParent(&m_root);

	return go;
}

void Scene::Load()
{
	json j;
	ReadJson(m_filepath, j);

	json& sceneGraph = j["SceneGraph"];
	ResourcesLoader::LoadSceneGraph(sceneGraph, *this);

	// Check for previous attached skybox
	Skybox* skybox = nullptr;
	Serialization::TryGetResource(j, "Skybox", skybox);
	SystemManager::GetRenderSystem().AttachNewSkybox(skybox);

	isLoaded = true;
}

void Scene::Unload()
{
	// Unload only if already loaded
	if (!isLoaded)
		return;

	// remove all gameObjects and components
	while (m_root.GetChildCount() != 0)
		DestroyGameObject(*m_root.GetChild(0), false);

	//m_gameObjects.clear();
	m_root.Clear();

	isLoaded = false;
}

void Scene::Serialize()
{
	json j;

	json& sceneGraph = j["SceneGraph"];
	ResourcesLoader::SaveSceneGraph(sceneGraph, &m_root);

	Skybox* curSkybox = SystemManager::GetRenderSystem().GetAttachedSkybox();
	j["Skybox"] = curSkybox ? std::string(curSkybox->GetUID()) : std::string("0");

	WriteJson(m_filepath, j);
}

void Scene::Deserialize()
{
	
}

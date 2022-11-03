#include "Resources/SceneManager.hpp"

#include "EngineContext.hpp"
#include "GameContext.hpp"

#include "Resources/Loaders/ResourcesLoader.hpp"
#include "Resources/ResourcesManager.hpp"
#include "Physics/PhysicsSystem.hpp"
#include "Tools/PathConfig.hpp"
#include "Core/TaskQueue.hpp"


void SceneManager::Initialize()
{
	
}

GameObject* SceneManager::FindGameObjectWithName(const std::string& name)
{
	Scene* scene = EngineContext::Instance().sceneManager->GetCurrentScene();
	if (scene)
		return scene->GetGameObjectFromName(name.c_str());

	return nullptr;
}

void SceneManager::DestroyGameObject(GameObject& GO)
{
	EngineContext& context = EngineContext::Instance();

	Scene* scene = context.sceneManager->GetCurrentScene();
	if (scene)
	{
		// Avoid destroy multiple time the same gameobject
		if (GO.IsDestroyed())
			return;

		GO.SetAsDestroyed();

		context.taskQueue->AddPriorityTask([scene, &GO]() {
			scene->DestroyGameObject(GO, true);
			});

		return;
	}

	Logger::Error("SceneManager - Impossible to destroy the GameObject '" + GO.GetName() + "', no active current scene");
	return;
}

void SceneManager::LoadExistingSceneOrCreateDefault()
{
	// if project empty, load default scene
	if (m_scenes.empty())
		CreateNewScene("SampleScene", ASSETS_ROOT_RAW);

	Scene* curScene = GetCurrentScene();
	curScene->Load();
}

void SceneManager::ChangeCurrentScene(unsigned int newSceneIndex)
{
	Scene* curScene = GetCurrentScene();
	Scene* newScene = m_scenes[newSceneIndex];

	// Check if there is actually a current scene and unload it
	if (curScene != nullptr)
		curScene->Unload();

	// Set new current scene and load
	m_curSceneIndex = newSceneIndex;
	newScene->Load();
}

void SceneManager::ChangeCurrentScene(Scene* scene)
{
	if (scene == nullptr) return;

	SceneManager* SM = EngineContext::Instance().sceneManager;
	for (unsigned int i = 0; i < SM->m_scenes.size(); i++)
	{
		if (SM->m_scenes[i] == scene)
		{
			SM->ChangeCurrentScene(i);
			return;
		}
	}

	// Should not happen
	Logger::Error("SceneManager - Can't load " + scene->GetFilename() + ", not registered");
}

void SceneManager::LoadNewCurrentScene(const std::string& sceneName)
{
	SceneManager* SM = EngineContext::Instance().sceneManager;
	for (unsigned int i = 0; i < SM->m_scenes.size(); i++)
	{
		if (SM->m_scenes[i]->GetName().compare(sceneName) == 0)
		{
			GameContext::Stop();
			SM->ChangeCurrentScene(i);
			GameContext::Start();
			return;
		}
	}

	Logger::Error("SceneManager - Can't load " + sceneName + ", not registered");
}

void SceneManager::LoadNewCurrentScene(Scene* scene)
{
	if (scene == nullptr) return;

	SceneManager* SM = EngineContext::Instance().sceneManager;
	for (unsigned int i = 0; i < SM->m_scenes.size(); i++)
	{
		if (SM->m_scenes[i] == scene)
		{
			GameContext::Stop();
			SM->ChangeCurrentScene(i);
			GameContext::Start();
			return;
		}
	}

	// Should not happen
	Logger::Error("SceneManager - Can't load " + scene->GetFilename() + ", not registered");
}

GameObject* SceneManager::DuplicateGameObject(GameObject& GO)
{
	Scene* scene = EngineContext::Instance().sceneManager->GetCurrentScene();
	if (scene)
		return &scene->DuplicateGameObject(GO);

	return nullptr;
}

void SceneManager::Register(Scene* scene)
{
	m_scenes.emplace_back(scene);
}

void SceneManager::Unregister(Scene* scene)
{
	auto it = std::find(m_scenes.begin(), m_scenes.end(), scene);
	if (it == m_scenes.end())
	{
		// Should not happen
		Logger::Warning("SceneManager - Scene " + scene->GetFilename() + " is not registered");
		return;
	}

	if (m_curSceneIndex == m_scenes.size() - 1 && m_curSceneIndex != 0) m_curSceneIndex--;

	m_scenes.erase(it);
}

Scene* SceneManager::CreateNewScene(const std::string& sceneFullPath)
{
	// Create and register scene to the scene manager
	Scene* newScene = EngineContext::Instance().resourcesManager->CreateResource<Scene>();
	//newScene->Resource::Load(scenePath + sceneName + ".scene");
	newScene->SetFileInfo(sceneFullPath);
	ResourcesLoader::CreateResourceFiles(newScene);

	return newScene;
}

Scene* SceneManager::CreateNewScene(const std::string& sceneName, const std::string& scenePath)
{
	// Create and register scene to the scene manager
	Scene* newScene = EngineContext::Instance().resourcesManager->CreateResource<Scene>();
	
	newScene->SetFileInfo(scenePath + "\\" + sceneName + ".scene");
	ResourcesLoader::CreateResourceFiles(newScene);

	return newScene;
}

Scene* SceneManager::ReloadCurrentScene()
{
	Scene* curScene = GetCurrentScene();

	if (curScene == nullptr)
	{
		Logger::Error("SceneManager - There is no current scene");
		return nullptr;
	}

	//	Reload curretn scene
	curScene->Unload();

	// Reset physics simulation to default
	SystemManager::GetPhysicsSystem().StopPhysics();
	SystemManager::GetPhysicsSystem().CreateSimulation();

	curScene->Load();

	return curScene;
}

void SceneManager::SaveCurrentScene()
{
	if (Scene* scene = GetCurrentScene())
		scene->Serialize();
}

GameObject* SceneManager::Instantiate(const std::string& name, const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
	if (Scene* scene = EngineContext::Instance().sceneManager->GetCurrentScene())
		return scene->Instantiate(name, nullptr, position, rotation, scale);
	return nullptr;
}

GameObject* SceneManager::Instantiate(const std::string& name, GameObject* parent, const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
	if (Scene* scene = EngineContext::Instance().sceneManager->GetCurrentScene())
		return scene->Instantiate(name, parent, position, rotation, scale);
	return nullptr;
}

GameObject* SceneManager::Instantiate(Prefab* prefab, const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
	if (Scene* scene = EngineContext::Instance().sceneManager->GetCurrentScene())
		return scene->Instantiate(prefab, nullptr, position, rotation, scale);
	return nullptr;
}

GameObject* SceneManager::Instantiate(Prefab* prefab, GameObject* parent, const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
	if (Scene* scene = EngineContext::Instance().sceneManager->GetCurrentScene())
		return scene->Instantiate(prefab, parent, position, rotation, scale);
	return nullptr;
}

void SceneManager::UpdateGameObjectTransforms()
{
	if (m_scenes.empty()) return;

	GameObject& root = m_scenes[m_curSceneIndex]->GetRoot();

	for (auto& child : root.GetChildren())
		child->UpdateTransforms();
}

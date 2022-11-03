#pragma once

#include "Resources/Resource/Resource.hpp"

#include "ECS/GameObject.hpp"
#include "EngineDLL.hpp"

#include <nlohmann/json.hpp>

#include "Generated/Scene.rfkh.h"

class Prefab;

/**
@brief Scene contains GameObjects (in a Scene Graph form)
*/
class HY_CLASS() Scene : public Resource
{
	friend class SceneManager;

private:
	HY_METHOD(rfk::Instantiator)
		static rfk::UniquePtr<Scene> defaultInstantiator(const HYGUID & uid);

private:
	GameObject m_root;
	std::unordered_map<HYGUID, std::unique_ptr<GameObject>> m_gameObjects;

	bool isLoaded = false;

protected:
	json m_jsonGraph;

public:
	ENGINE_API Scene(const HYGUID& uid);
	~Scene();

private:
	/**
	Erase a gameobject on the scene

	@param GO : GameObject to remove
	*/
	void DestroyGameObject(GameObject & GO, bool shouldCallOnDestroy);

public:
	/**
	@brief Instantiate a prefab in the scene and return the gameObject created

	@param Prefab* : prefab to instantiate
	@return GameObject* : gameObject created
	*/
	GameObject* Instantiate(Prefab* prefab);

	/**
	@brief Instantiate a prefab in the scene and return the gameObject created
	
	@param Prefab* : prefab to instantiate
	@param GameObject* : parent
	@param Vector3 : position
	@param Quaternion : rotation
	@param Vector3 : scale
	@return GameObject* : gameObject created
	*/
	GameObject* Instantiate(Prefab* prefab, GameObject* parent = nullptr, Vector3 position = Vector3::Zero, Quaternion rotation = Quaternion::Identity, Vector3 scale = Vector3::One);

	/**
	@brief Instantiate a copy of a gameobject in the scene and return the gameObject created

	@param GameObject* : gameObject to instantiate
	@param GameObject* : parent
	@param Vector3 : position
	@param Quaternion : rotation
	@param Vector3 : scale
	@return GameObject* : gameObject created
	*/
	GameObject* Instantiate(const std::string& name, GameObject* parent = nullptr, Vector3 position = Vector3::Zero, Quaternion rotation = Quaternion::Identity, Vector3 scale = Vector3::One);
	
	/**
	@brief Get the scene root (empty GameObject)

	@return GameObject& : gameObject reference
	*/
	ENGINE_API GameObject& GetRoot();

	/**
	@brief Load the scene from json parsed values
	*/
	ENGINE_API void Load();

	/**
	@brief Unload the scene, destroy all objects
	*/
	ENGINE_API void Unload();

	/**
	@brief Get the json representing the scene graph
	
	@return json& : json graph
	*/
	json& GetJsonGraph();

	/**
	@brief Get a GameObject with its GUID

	@return GameObject* : gameObject pointer if found, nullptr otherwise
	*/
	GameObject& DuplicateGameObject(GameObject& GO);

	/**
	@brief Get a GameObject with its GUID

	@return GameObject* : gameObject pointer if found, nullptr otherwise
	*/
	ENGINE_API GameObject* GetGameObject(const HYGUID& uid);

	/**
	@brief Get a GameObject with its name (use with precaution)

	@return GameObject* : gameObject pointer if found, nullptr otherwise
	*/
	GameObject* GetGameObjectFromName(const HyString& name);

	/**
	@brief Create a new GameObject (or re-create one with an already existing GUID)

	@return GameObject* : gameObject created
	*/
	GameObject* CreateGameObject(const std::string& name = "NewGameObject", const HYGUID& uid = HYGUID::NewGUID());


	virtual void Serialize() override;
	virtual void Deserialize() override;

	Scene_GENERATED
};

File_Scene_GENERATED
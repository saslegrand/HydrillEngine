#pragma once
#include <vector>

#include "Resources/Resource/Scene.hpp"
#include "EngineDLL.hpp"

class ENGINE_API SceneManager
{
private:
#pragma warning (disable:4251)
	std::vector<Scene*> m_scenes;
#pragma warning (default:4251)
	unsigned int m_curSceneIndex = 0;

	void ChangeCurrentScene(unsigned int newIndex);

public:

	void Initialize();

	// -------------
	// Manage Scenes

	/**
	@brief Register a new scene to the system

	@param scene : Scene to register
	*/
	void Register(Scene* scene);

	/**
	@brief Unregister a scene from the system

	@param scene : Scene to unregister
	*/
	void Unregister(Scene* scene);

	/**
	@brief Change the current scene (non-game purpose)

	@param scene : Scene to set active
	*/
	void ChangeCurrentScene(Scene* scene);

	/**
	@brief Create a new empty scene

	@param sceneFullPath : full path of the scene
	*/
	Scene* CreateNewScene(const std::string& sceneFullPath);

	/**
	@brief Load first scene, if empty create a new scene
	*/
	void LoadExistingSceneOrCreateDefault();

	/**
	@brief Create a new empty scene
	
	@param sceneName : const std::string& ("DefaultScene" by default)
	@return scenePath : local directory of the new scene

	@return Scene* : pointer to the created scene
	*/
	Scene* CreateNewScene(const std::string& sceneName, const std::string& scenePath);

	/**
	@brief Create a new empty scene

	@param sceneName : const std::string& ("DefaultScene" by default)
	@return Scene* : pointer to the current scene, nullptr if no scene
	*/
	Scene* GetCurrentScene() const;

	/**
	@brief Reload current scene

	@return Scene* : pointer to the current scene, nullptr if no scene
	*/
	Scene* ReloadCurrentScene();

	/**
	@brief Update all transforms recursively on the scene graph
	*/
	void UpdateGameObjectTransforms();

	/**
	@brief Save the current scene (serialize itself in its asset file)
	*/
	void SaveCurrentScene();

	/**
	@brief Return a GameObject ref from it's name

	@param name : Name of the GameObject to return

	@return GameObject* : GameObject found, nullptr otherwise
	*/
	static GameObject* FindGameObjectWithName(const std::string& name);

	/**
	@brief Destroy a GameObject from the current scene

	@param GO : GameObject to destroy
	*/
	static void DestroyGameObject(GameObject& GO);

	/**
	@brief Change the current scene running in the game, from name

	@param name : Name of the Scene to load
	*/
	static void LoadNewCurrentScene(const std::string& sceneName);

	/**
	@brief Change the current scene running in the game, from Scene reference

	@param scene : Scene to load
	*/
	static void LoadNewCurrentScene(Scene* scene);

	/**
	@brief Duplicate a selected GameObject in the current scene

	@param GO : GameObject to duplicate
	*/
	static GameObject* DuplicateGameObject(GameObject& GO);

	// ----------------------------
	// New GameObject Instantiators

	/**
	@brief Instantiate a new empty GameObject

	@param const std::string& : name
	@param const Vector3& : position
	@param const Quaternion& : rotation
	@param const Vector3& : scale
	@return GameObject* : gameObject instanciated
	*/
	static GameObject* Instantiate(const std::string& name = "NewGameObject", const Vector3 & position = Vector3::Zero, const Quaternion & rotation = Quaternion::Identity, const Vector3 & scale = Vector3::One);

	/**
	@brief Instantiate a new empty GameObject, providing a parent

	@param const std::string& : name
	@param GameObject* : parent
	@param const Vector3& : position
	@param const Quaternion& : rotation
	@param const Vector3& : scale
	@return GameObject* : gameObject instanciated
	*/
	static GameObject* Instantiate(const std::string& name, GameObject* parent, const Vector3& position = Vector3::Zero, const Quaternion& rotation = Quaternion::Identity, const Vector3& scale = Vector3::One);
	

	// ------------------------
	// New Prefab Instantiators

	/**
	@brief Instantiate a new GameObject from a prefab instance

	@param Prefab* : prefab to instantiate
	@param const Vector3& : position
	@param const Quaternion& : rotation
	@param const Vector3& : scale
	@return GameObject* : gameObject instanciated
	*/
	static GameObject* Instantiate(Prefab* prefab, const Vector3& position = Vector3::Zero, const Quaternion& rotation = Quaternion::Identity, const Vector3& scale = Vector3::One);
	
	
	/**
	@brief Instantiate a new GameObject from a prefab instance, provinding a parent

	@param Prefab* : prefab to instantiate
	@param GameObject* : parent
	@param const Vector3& : position
	@param const Quaternion& : rotation
	@param const Vector3& : scale
	@return GameObject* : gameObject instanciated
	*/
	static GameObject* Instantiate(Prefab* prefab, GameObject* parent, const Vector3& position = Vector3::Zero, const Quaternion& rotation = Quaternion::Identity, const Vector3& scale = Vector3::One);
};

#include "Resources/SceneManager.inl"

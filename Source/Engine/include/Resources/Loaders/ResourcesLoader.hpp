#pragma once

#include <string>

#include "Resources/Parsers/AssimpParser.hpp"
#include "Tools/PathConfig.hpp"

#include "Resources/Resource/Model.hpp"
#include "Resources/Resource/Texture.hpp"
#include "Resources/Resource/Shader.hpp"
#include "Resources/Resource/Scene.hpp"
#include "Resources/Resource/Material.hpp"
#include "Resources/Resource/Terrain.hpp"
#include "EngineDLL.hpp"

template <typename OBJ>
using RefContainer = std::unordered_map<HYGUID, OBJ>;

enum class ExtensionType
{
	META,
	RESOURCE,
	MODEL,
	TEXTURE,
	SOUND,
	IGNORE,
	UNKNOWN
};

class GameObject;
class ResourcesManager;

namespace std::filesystem
{
	class path;
}

/**
@brief Resources Loader handle resource loading and file writing & reading
*/
class ResourcesLoader
{
private:
	/**
	@brief Serialize a Scene Graph object

	@param json& : json tree to serialize in
	@param GameObject* : gameObject to serialize
	*/
	static void SaveGameObjectRecurse(json& j, const GameObject& gameObject);

	/**
	@brief Local function to save a GameEntity

	@param json& : Json tree to serialize in
	@param rfkClass : Entity class archetype
	@param object : Entity
	*/
	static void SaveGameEntity(json& j, rfk::Class const& rfkClass,  BaseObject* object);

	/**
	@brief Local function to save a GameEntity

	@param json& : Json tree to serialize in
	@param fieldName : Name of the entity
	@param fieldArch : Entity archetype
	@param fieldData : Entity data
	@param isPointer : Is the entity initially a reference
	*/
	static void SaveGameEntity(json& jsonFile, const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, bool isPointer);

	/**
	@brief Local function to save a game template entity

	@param json& : Json tree to serialize in
	@param fieldName : Name of the entity
	@param cTempInstance : Template entity class archetype
	@param fieldData : Template entity data
	*/
	static void SaveGameTemplateEntity(json& jsonFile, const char* fieldName, rfk::ClassTemplateInstantiation const* cTempInstance, void* fieldData);

	/**
	@brief Local a gameObject in the given scene

	@param json& : Json tree to serialize in
	@param scene : Scene where to load the GO
	@param shouldCreateNewId : If true, GO and its components will have new uIds. If false, reuse saved uIds
	*/
	static GameObject* LoadGameObjects(json& j, Scene& scene, bool shouldCreateNewId);

	/**
	@brief Local function to load a game entity

	@param json& : Json tree to serialize in
	@param objRefs : Saved objects (use for retrieve ref from uIds)
	@param fieldArch : Entity archetype
	@param fieldData : Template entity data
	@param isPointer : Is the entity initially a reference
	*/
	static void LoadGameEntity(json& jsonFile, RefContainer<SceneObject*>& objRefs, rfk::Archetype const* fieldArch, void* fieldData, bool isPointer);

	/**
	@brief Local function to load a game entity

	@param json& : Json tree to serialize in
	@param objRefs : Saved objects (use for retrieve ref from uIds)
	@param rfkClass : Entity class archetype
	@param object : Entity
	*/
	static void LoadGameEntity(json& jsonFile, RefContainer<SceneObject*>& objRefs, rfk::Class const& rfkClass, BaseObject* object);

	/**
	@brief Local function to load a game template entity

	@param json& : Json tree to serialize in
	@param objRefs : Saved objects (use for retrieve ref from uIds)
	@param cTempInstance : Template entity class archetype
	@param fieldData : Template entity data
	*/
	static void LoadGameTemplateEntity(json& jsonFile, RefContainer<SceneObject*>& objRefs, rfk::ClassTemplateInstantiation const* cTempInstance, void* fieldData);

	/**
	@brief Parse directory at 'path' and check if there is a resource that is not loaded (no meta file present)

	@param path : Directory to load in
	*/
	static void LoadNotCachedRecurse(const std::filesystem::path& path, bool& foundResource);

public:

	/**
	@brief Load all default resources that have already been loaded with the engine, by reading all .meta files and rebuilding the resources associated 

	@param path : Directory to load in
	*/
	static ENGINE_API void LoadResourcesInDirectory(std::string_view path);

	/**
	Load all the not cached resources in the given directory. (no meta file associated)
	Resources are loaded in multiple threads. This function will not return until all tasks are completed.

	@param path : Directory to load in
	*/
	static ENGINE_API void LoadNotCachedResources(std::string_view path);


	//void LoadDefaultResources();

	/**
	@brief Load all the resources that have already been loaded with the engine, by reading all .meta files and rebuilding the resources associated
	*/
	//static void LoadProjectResources();

	/**
	@brief Create .meta & cache file for a resource

	@param Resource* : resource
	*/
	static ENGINE_API void CreateResourceFiles(Resource* resource);

	/**
	@brief Delete .meta & cache file for a resource

	@param Resource* : resource
	*/
	static void DeleteResourceFiles(Resource* resource);


	/**
	@brief Save a Scene Graph for its json serialization

	@param json& : json tree to serialize in
	@param GameObject* : gameObject scene root
	*/
	static void SaveSceneGraph(json& j, GameObject* root);

	/**
	@brief Save a Prefab from a gameObject root, for its json serialization

	@param json& : json tree to serialize in
	@param Prefab& : prefab to build in
	@param GameObject* : gameObject root
	*/
	static void SavePrefab(Prefab& prefab, GameObject& root);

	/**
	@brief Load a Scene Graph from its json pattern

	@param json& : json tree to deserialize from
	@param Scene& : scene to spawn the scene graph in
	*/
	static GameObject* LoadSceneGraph(json& j, Scene& scene);
	
	/**
	@brief Load a prefab from its json pattern into a scene

	@param json& : json tree to load from
	@return Prefab& : prefab to load in the scene
	@param Scene& : scene to spawn the prefab in
	*/
	static GameObject* LoadPrefab(Prefab& prefab, Scene& scene);

	/**
	@brief Load a model from its filepath with ASSIMP

	@param Model& : model reference
	@return bool : true if success, false if failure 
	*/
	static bool LoadModel(Model& model);

	/**
	@brief Load a texture from its filepath with STB_IMAGE

	@param Texture& : texture reference
	@return bool : true if success, false if failure
	*/
	static ENGINE_API bool LoadTexture(Texture& texture);


	/**
	@brief Load a texture from its filepath with STB_IMAGE, data not free

	@param Texture& : texture reference
	@return bool : true if success, false if failure
	*/
	static ENGINE_API bool LoadTextureUnsafe(Texture& texture);

	/**
	@brief Load a texture from its filepath with STB_IMAGE, data not free

	@param Texture& : texture reference
	@return bool : true if success, false if failure
	*/
	static ENGINE_API void FreeTextureData(Texture& texture);

	/**
	@brief Load a terrain to create its mesh from its attached texture

	@param Terrain& : terrain reference
	@param bool : force a complete reload
	@return bool : true if success, false if failure
	*/
	static bool LoadTerrain(Terrain& terrain, bool forceReload);




private:
	static const std::unordered_map<std::string, ExtensionType> ExtensionTypeMap;

public:
	/**
	@brief Get file type according to extension
	
	@param extension : const std::string& (should be ".extension" like)
	@return ExtensionType : enum describing type of file
	*/
	static ENGINE_API ExtensionType GetExtensionType(const std::string& extension);
};

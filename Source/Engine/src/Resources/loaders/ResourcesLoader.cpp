#include "Resources/Loaders/ResourcesLoader.hpp"

#include <iostream>
#include <stdio.h>
#include <fstream>

#include <assimp/postprocess.h>
#include <glad/gl.h>

#include "EngineContext.hpp"

#include "Resources/ResourcesManager.hpp"
#include "Resources/Parsers/AssimpParser.hpp"

#include "Tools/SerializationUtils.hpp"
#include "Tools/StringHelper.hpp"
#include "Tools/PathConfig.hpp"
#include "Tools/RFKUtils.hpp"

#include "ECS/GameObject.hpp"

#include "Physics/PhysicsSimulation.hpp"

inline std::string GetFileString(const std::string& filepath)
{
	std::ifstream ifs(filepath);
	return std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
}

void ResourcesLoader::LoadNotCachedRecurse(const std::filesystem::path& path, bool& foundResource)
{
	std::unordered_map<std::string, std::string> newPotentialObjects;
	std::unordered_map<std::string, int> cachedObject;

	for (const auto& file : fs::directory_iterator(path))
	{
		if (fs::is_directory(file.path()))
		{
			LoadNotCachedRecurse(file.path(), foundResource);
			continue;
		}

		std::string filepath = file.path().string();
		std::string ext = file.path().extension().string();
		ExtensionType extType = ResourcesLoader::GetExtensionType(ext);


		if (extType == ExtensionType::RESOURCE)
		{
			// Add to cached object list
			cachedObject.insert({ filepath, 0 });

			// Remove resource path if already added
			auto it = newPotentialObjects.find(filepath);
			if (it != newPotentialObjects.end())
				newPotentialObjects.erase(it);
		}
		else if (extType == ExtensionType::MODEL || extType == ExtensionType::TEXTURE || extType == ExtensionType::SOUND)
		{
			std::string resPath = StringHelper::GetFilePathWithoutExtension(filepath);
			if (extType == ExtensionType::MODEL) resPath += ".model";
			else if (extType == ExtensionType::TEXTURE) resPath += ".texture";
			else if (extType == ExtensionType::SOUND) resPath += ".sound";
			auto it = cachedObject.find(resPath);
			if (it == cachedObject.end())
				newPotentialObjects.insert({ resPath, filepath });
		}
	}

	if (newPotentialObjects.size() != 0)
	{
		foundResource = true;

		uint32_t textureCount = 0, modelCount = 0, soundCount = 0;
		for (auto [key, path] : newPotentialObjects)
		{
			const std::string ext = StringHelper::GetFileExtensionFromPath(key);
			if (ext == ".model")		modelCount++;
			else if (ext == ".texture") textureCount++;
			else if (ext == ".sound")	soundCount++;
		}

		Logger::Info("=> Found : " + std::to_string(modelCount) + " models, " + std::to_string(textureCount) + " textures, " + std::to_string(soundCount) + " sounds at :");
		Logger::Info("=> Directory : " + path.string());
	}

	ResourcesManager* RM = EngineContext::Instance().resourcesManager;
	ResourcesTaskPool& taskPool = RM->GetTaskPool();

	for (auto [key, path] : newPotentialObjects)
	{
		if (RM->GetResourceByPath(path) == nullptr)
			taskPool.AddMultiThreadTask([RM, path]() { RM->ImportNewResource(path); });
	}
}

void ResourcesLoader::LoadNotCachedResources(std::string_view path)
{
	ResourcesTaskPool& taskPool = EngineContext::Instance().resourcesManager->GetTaskPool();

	bool foundResource = false;
	Logger::Info("=> Check for new resources");
	LoadNotCachedRecurse(path, foundResource);

	if (!foundResource)
		Logger::Info("=> No new resource found");

	Logger::Info("=> Load new resources success");

	// Wait for loading tasks to be completed
	taskPool.WaitingForMultiTasksCompletion();

	// Run post multi tasks
	taskPool.RunSingleTasks();
}

void ResourcesLoader::LoadResourcesInDirectory(std::string_view path)
{
	ResourcesManager* RM = EngineContext::Instance().resourcesManager;

	std::vector<Resource*> newResources;

	Logger::Info("=> Loading Project Resources...");
	Logger::Info("=> Directory : " + std::string(path.data()));
	Logger::Info("=> Meta import");

	uint32_t resourceCount = 0;
	for (const auto& file : fs::recursive_directory_iterator(path))
	{
		if (fs::is_directory(file.path()))
			continue;

		std::string extension = file.path().extension().string();
		ExtensionType extType = ResourcesLoader::GetExtensionType(extension);

		// TODO : Check if a meta file with this name already exists, else load has a new entry resource
		if (extType != ExtensionType::META)
			continue;

		// Load the file
		std::ifstream i(file.path().string());
		if (!i.is_open())
			continue;

		// Parse the file
		json j = json::parse(i);
		i.close();

		// Should not happen
		if (!j.contains("uid") || !j.contains("type"))
			continue;

		// Get resource internal information
		std::string typeString = j["type"].get<std::string>();
		std::string uid = j["uid"].get<std::string>();

		HYGUID guid = HYGUID(uid);

		Resource* resource = RM->CreateResource(typeString, guid);
		resource->SetFileInfo(StringHelper::GetFilePathWithoutExtension(file.path().string()));

		newResources.emplace_back(resource);
		resourceCount++;
	}

	if (resourceCount != 0)
	{
		Logger::Info("=> Found " + std::to_string(resourceCount) + " cached resources");
		Logger::Info("=> Reloading from cache");

		for (auto& resource : newResources)
			resource->Deserialize(); // LOAD ALL FILES

		Logger::Info("=> Creating GPU memory");
		for (auto& resource : newResources)
			resource->LoadInGPUMemory();
	}
	else
	{
		Logger::Info("=> No cached resource found");
	}

	// Load all new resources
	LoadNotCachedResources(path);

	Logger::Info("=> Load resources folder success");
}

void ResourcesLoader::CreateResourceFiles(Resource* resource)
{
	if (resource == nullptr) 
		return;

	if (resource->GetFilepath() == "")
		return;

	// Create json data
	json j;

	j["uid"] = std::string(resource->GetUID()); // Store uid
	j["type"] = resource->GetClassName();		// Store resource class name

	// Create meta file
	std::string filepath = resource->GetFilepath() + ".meta";
	std::ofstream o(filepath);

	// Write JSON into it
	o << std::setw(1) << j << std::endl;

	o.close();

	// Serialize resource on disk
	resource->Serialize();
}

void ResourcesLoader::DeleteResourceFiles(Resource* resource)
{
	// Try remove resource files
	if (std::filesystem::exists(resource->GetFilepath()))
		std::filesystem::remove(resource->GetFilepath());

	std::filesystem::path metaPath = resource->GetFilepath() + ".meta";
	if (std::filesystem::exists(metaPath))
		std::filesystem::remove(metaPath);
}

bool ResourcesLoader::LoadModel(Model& model)
{
	// TODO : We could improve that and provide some kind of ImportSettings
	return AssimpParser::LoadModel(model, aiPostProcessSteps::aiProcess_Triangulate
									| aiPostProcessSteps::aiProcess_GenSmoothNormals
									| aiPostProcessSteps::aiProcess_CalcTangentSpace
									| aiPostProcessSteps::aiProcess_JoinIdenticalVertices
									| aiPostProcessSteps::aiProcess_GenBoundingBoxes);
								//| aiPostProcessSteps::aiProcess_FlipUVs // This is only used by DirectX
								//| aiPostProcessSteps::aiProcess_GlobalScale // This is supposed to get fbx scale but most fbx don't use it
								//| aiPostProcessSteps::aiProcess_ValidateDataStructure
}

bool ResourcesLoader::LoadTexture(Texture& texture)
{
	return AssimpParser::LoadTexture(texture, texture.GetLoadingFlags(), true);
}

bool ResourcesLoader::LoadTextureUnsafe(Texture& texture)
{
	return AssimpParser::LoadTexture(texture, texture.GetLoadingFlags(), false);
}

void ResourcesLoader::FreeTextureData(Texture& texture)
{
	AssimpParser::FreeTexture(texture);
	texture.data.data = nullptr;
}

bool ResourcesLoader::LoadTerrain(Terrain& terrain, bool forceReload)
{
	Mesh* heightMesh = terrain.heightMapMesh;
	if (!forceReload && heightMesh != nullptr)
	{
		// If we already have a mesh, and we dont want to force reload, no need to reload
		return true;
	}

	Texture* heightMap = terrain.heightMapTexture;
	if (!heightMap)
		return false;

	if (!LoadTextureUnsafe(*heightMap))
		return false;

	bool reloadingMesh = terrain.heightMapMesh;

	if (!reloadingMesh) // maybe we are reloading a mesh, we just don't have it, so look for it
	{
		terrain.heightMapMesh = static_cast<Mesh*>(EngineContext::Instance().resourcesManager->GetResourceByPath(StringHelper::GetFilePathWithoutExtension(terrain.GetFilepath()) + ".mesh"));
		if (terrain.heightMapMesh)
		{
			terrain.heightMapMesh->UnloadFromGPUMemory();
			reloadingMesh = true;
		}
	}
		
	if (!terrain.heightMapMesh)
		terrain.heightMapMesh = EngineContext::Instance().resourcesManager->CreateResource<Mesh>(StringHelper::GetFilePathWithoutExtension(terrain.GetFilepath()) + ".mesh");

	if (!PhysicsSimulation::GenerateTerrain(&terrain))
		return false;

	terrain.heightMapMesh->Serialize();

	if (reloadingMesh)
		terrain.heightMapMesh->LoadInGPUMemory();

	terrain.Serialize();

	FreeTextureData(*heightMap);
	return true;
}


#pragma region SCENE_GRAPH_DE/SERIALIZATION

// Serialize internal data of a given gameobject
void SaveInternalGameObject(json& goJson, const GameObject& gameObject)
{
	goJson["m_name"] = gameObject.GetName();
	goJson["m_parent"] = gameObject.GetParent() ? std::string(gameObject.GetParent()->GetUID()) : "0";
	goJson["m_isActive"] = gameObject.IsActive();

	json& transformJson = goJson["transform"];

	transformJson["isStatic"] = gameObject.isStatic;
	Vector3 position = gameObject.transform.LocalPosition();
	Serialization::SetContainer<float>(transformJson["position"], position);
	Quaternion rotation = gameObject.transform.LocalRotation();
	Serialization::SetContainer<float>(transformJson["rotation"], rotation);
	Vector3 scale = gameObject.transform.LocalScale();
	Serialization::SetContainer<float>(transformJson["scale"], scale);
}

// Deserialize internal data of a given gameobject
GameObject* LoadInternalGameObject(json& goJson, Scene& scene, const HYGUID& uid)
{
	std::string name;
	Serialization::TryGetValue(goJson, "m_name", name);
	GameObject* GO = scene.CreateGameObject(name, uid);
	
	bool active = true;
	Serialization::TryGetValue(goJson, "m_isActive", active);
	GO->SetActive(active);

	json& tr = goJson["transform"];
	Serialization::TryGetValue(tr, "isStatic", GO->isStatic);

	Vector3 pos		= Vector3::Zero;
	Vector3 sca		= Vector3::One;
	Quaternion rot  = Quaternion::Identity;

	Serialization::TryGetContainer<float>(tr, "position", pos);
	Serialization::TryGetContainer<float>(tr, "rotation", rot);
	Serialization::TryGetContainer<float>(tr, "scale", sca);

	GO->transform.SetLocalPosition(pos);
	GO->transform.SetLocalRotation(rot);
	GO->transform.SetLocalScale(sca);

	return GO;
}

void ResourcesLoader::SaveGameTemplateEntity(json& jsonFile, const char* fieldName, rfk::ClassTemplateInstantiation const* cTempInstance, void* fieldData)
{
	// Check if the emplate class is a vector
	if (cTempInstance->hasSameName("HyVector"))
	{
		// Find type and archetype of the template instantiation
		rfk::TemplateArgument const& tempArg = cTempInstance->getTemplateArgumentAt(0);
		rfk::TypeTemplateArgument const& typeTempArg = *static_cast<const rfk::TypeTemplateArgument*>(&tempArg);
		rfk::Type const& typeTemp = typeTempArg.getType();
		rfk::Archetype const* valueTypeArch = typeTemp.getArchetype();

		// Class not reflected, not supported
		if (valueTypeArch == nullptr)
			return;

		json& valueJson = jsonFile[fieldName];

		// Get vetor methods
		std::size_t vecSize = cTempInstance->getMethodByName("size")->invokeUnsafe<std::size_t>(fieldData);
		rfk::Method const* getMethod = cTempInstance->getMethodByName("GetElementAt");

		// Save vector values
		for (std::size_t i = 0; i < vecSize; i++)
		{
			void* value = getMethod->invokeUnsafe<void*>(fieldData, static_cast<std::size_t>(i));

			std::string valueName = "v" + std::to_string(i);
			SaveGameEntity(valueJson, valueName.c_str(), valueTypeArch, value, typeTemp.isPointer());
		}
	}
	
	else if (cTempInstance->hasSameName("Flags"))
	{
		rfk::TypeTemplateArgument const& typeTempArg = *static_cast<const rfk::TypeTemplateArgument*>(&cTempInstance->getTemplateArgumentAt(0));
		rfk::Enum const* enumField = rfk::enumCast(typeTempArg.getType().getArchetype());

		if (enumField == nullptr)
			return;

		json& valueJson = jsonFile[fieldName];

		rfk::Method const* getMethod = cTempInstance->getMethodByName("GetFlagAsInt");
		valueJson = getMethod->invokeUnsafe<int64_t>(fieldData);
	}
}

// Deserialize GO comps for scene/prefab
void ResourcesLoader::SaveGameEntity(json& jsonFile, const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, bool isPointer)
{
	if (fieldArch == nullptr)
		return;

	json& fieldValuesJson = jsonFile[fieldName];

	// Set value from known type (fundamental c++, Maths, color, string)
	if (RFKJsonSaveMap::SetValueFromType(fieldValuesJson, fieldArch, fieldData))
		return;

	// Save enum value
	if (rfk::enumCast(fieldArch))
	{
		fieldValuesJson = *static_cast<rfk::int16*>(fieldData);
		return;
	}

	// Check if the value is a template class
	rfk::ClassTemplateInstantiation const* cTempInstance = rfk::classTemplateInstantiationCast(fieldArch);
	if (cTempInstance)
	{
		SaveGameTemplateEntity(jsonFile, fieldName, cTempInstance, fieldData);
		return;
	}

	// Check if the field is a class/struct (other types not supported for now)
	rfk::Class const* fieldClass = rfk::classCast(fieldArch);
	if (fieldClass == nullptr)
	{
		fieldClass = rfk::structCast(fieldArch);
		if (fieldClass == nullptr)
			return;
	}

	// Check if the field is a reference to another object
	if (!isPointer)
	{
		if (fieldClass->isSubclassOf(BaseObject::staticGetArchetype()))
			SaveGameEntity(fieldValuesJson, *fieldClass, reinterpret_cast<BaseObject*>(fieldData));
		return;
	}

	// Save scene object reference
	if (fieldClass->isSubclassOf(SceneObject::staticGetArchetype()))
	{
		SceneObject* obj = *static_cast<SceneObject**>(fieldData);
		fieldValuesJson = obj != nullptr ? std::string(obj->GetUID()) : std::string("0");
		return;
	}

	Logger::Warning("ResourcesLoader - Entity '" + std::string(fieldName) + "' is not supported");
	fieldValuesJson = std::string("UNKNOW_TYPE");
}

// Deserialize GO comps for scene/prefab
void ResourcesLoader::SaveGameEntity(json& jsonFile, rfk::Class const& rfkClass, BaseObject* object)
{
	auto fields = rfkClass.getFieldsByPredicate([](rfk::Field const& field, void* data) {
		return true;
		}, nullptr, true, true);

	// Loop on each field
	for (auto field : fields)
		SaveGameEntity(jsonFile, field->getName(),
			field->getType().getArchetype(),
			field->getPtr(*object),
			field->getType().isPointer());
}

#include "Resources/Resource/Sound.hpp"

// Deserialize GO comp refs for prefab/scene
void LoadGameEntityRef(json& jsonFile, RefContainer<SceneObject*>& objRefs, rfk::Class const& c, void* fieldData)
{
	if (c.isSubclassOf(Resource::staticGetArchetype()) || c.isBaseOf(Resource::staticGetArchetype()))
	{
		// Get saved uID
		std::string resId = "";
		Serialization::TryGetValue(jsonFile, resId, nlohmann::detail::value_t::string);

		// Retrieve resource pointer
		Resource** fieldRes = static_cast<Resource**>(fieldData);

		// Set resource
		*fieldRes = EngineContext::Instance().resourcesManager->GetResource(HYGUID(resId));
	}

	else if (c.isSubclassOf(SceneObject::staticGetArchetype()))
	{
		// Get saved uID
		std::string goId = "";
		Serialization::TryGetValue(jsonFile, goId, nlohmann::detail::value_t::string);

		if (goId.compare("0") != 0)
		{
			// Retrieve objects pointer
			SceneObject* object = static_cast<SceneObject*>(objRefs[HYGUID(goId)]);
			SceneObject** fieldObj = static_cast<SceneObject**>(fieldData);

			// Set object
			*fieldObj = object;
		}
	}
}

void ResourcesLoader::LoadGameTemplateEntity(json& jsonFile, RefContainer<SceneObject*>& objRefs, rfk::ClassTemplateInstantiation const* cTempInstance, void* fieldData)
{
	if (cTempInstance->hasSameName("HyVector"))
	{
		// Find type and archetype of the template instantiation
		rfk::TemplateArgument const& tempArg = cTempInstance->getTemplateArgumentAt(0);
		rfk::TypeTemplateArgument const& typeTempArg = *static_cast<const rfk::TypeTemplateArgument*>(&tempArg);
		rfk::Type const& typeTemp = typeTempArg.getType();
		rfk::Archetype const* valueTypeArch = typeTemp.getArchetype();

		// Class not reflected, not supported
		if (valueTypeArch == nullptr)
			return;

		bool isValuePointer = typeTemp.isPointer();

		// Retrieve vector instance
		STDTemplateClass& vecInstance = *reinterpret_cast<STDTemplateClass*>(fieldData);

		rfk::Method const* addElementMethod = cTempInstance->getMethodByName("AddNewElement");

		std::size_t index = 0;
		for (json::iterator valueIt = jsonFile.begin(); valueIt != jsonFile.end(); ++valueIt)
		{
			json& valueJsonFile = valueIt.value();

			void* value = addElementMethod->invokeUnsafe<void*>(fieldData);

			LoadGameEntity(valueJsonFile, objRefs, valueTypeArch, value, typeTemp.isPointer());

			index++;
		}
	}

	else if (cTempInstance->hasSameName("Flags"))
	{
		int64_t jsonValue = jsonFile.get<int64_t>();

		rfk::Method const* setMethod = cTempInstance->getMethodByName("SetFlagFromInt");
		setMethod->invokeUnsafe<int64_t>(fieldData, std::move(jsonValue));
	}
}

void ResourcesLoader::LoadGameEntity(json& jsonFile, RefContainer<SceneObject*>& objRefs, rfk::Archetype const* fieldArch, void* fieldData, bool isPointer)
{
	if (fieldArch == nullptr)
		return;

	if (RFKJsonLoadMap::LoadValueFromType(jsonFile, fieldArch, fieldData))
		return;

	// Load enum value
	if (rfk::enumCast(fieldArch))
	{
		*static_cast<rfk::int16*>(fieldData) = jsonFile.get<rfk::int16>();
		return;
	}

	// Check if the value is a template class
	rfk::ClassTemplateInstantiation const* cTempInstance = rfk::classTemplateInstantiationCast(fieldArch);
	if (cTempInstance)
	{
		LoadGameTemplateEntity(jsonFile, objRefs, cTempInstance, fieldData);
		return;
	}

	rfk::Class const* fieldClass = rfk::classCast(fieldArch);
	if (fieldClass == nullptr)
	{
		fieldClass = rfk::structCast(fieldArch);
		if (fieldClass == nullptr)
			return;
	}

	if (!isPointer)
	{
		if (fieldClass->isSubclassOf(BaseObject::staticGetArchetype()))
			LoadGameEntity(jsonFile, objRefs, *fieldClass, reinterpret_cast<BaseObject*>(fieldData));

		return;
	}

	LoadGameEntityRef(jsonFile, objRefs, *fieldClass, fieldData);
}

// Deserialize GO comps for prefab/scene
void ResourcesLoader::LoadGameEntity(json& jsonFile, RefContainer<SceneObject*>& objRefs, rfk::Class const& c, BaseObject* object)
{
	for (json::iterator entityValuesIt = jsonFile.begin(); entityValuesIt != jsonFile.end(); ++entityValuesIt)
	{
		const std::string fieldName = entityValuesIt.key();
		json& valueJson = entityValuesIt.value();

		// Check if the field always exists
		rfk::Field const* field = c.getFieldByName(fieldName.c_str(), rfk::EFieldFlags::Default, true);
		if (field != nullptr)
		{
			rfk::Type const& fieldType = field->getType();
			LoadGameEntity(valueJson, objRefs, fieldType.getArchetype(), field->getPtr(*object), fieldType.isPointer());
		}
	}
}

GameObject* ResourcesLoader::LoadGameObjects(json& j, Scene& scene, bool shouldCreateNewId)
{
	RefContainer<SceneObject*> objRefs;

	GameObject* root = nullptr;

	// Preload gameobjects and components
	for (json::iterator internalIt = j.begin(); internalIt != j.end(); ++internalIt)
	{
		json& goJson = internalIt.value();

		// Create and get internal gameobject data
		json& goFields = goJson["Internal"];

		// Create or re-use given ID
		HYGUID goIdSave = HYGUID(internalIt.key());
		HYGUID goId = shouldCreateNewId ? HYGUID::NewGUID() : goIdSave;
		GameObject* GO = LoadInternalGameObject(goFields, scene, goId);
		
		// Track gameobject for parenting and component ref in prefab
		objRefs.emplace(goIdSave, GO);

		// Create components
		json& components = goJson["Components"];
		for (json::iterator compIt = components.begin(); compIt != components.end(); ++compIt)
		{
			json& compJson = compIt.value();

			// Create component
			// Create or re-use given ID
			HYGUID compIdSave = HYGUID(compIt.key());
			HYGUID compId = shouldCreateNewId ? HYGUID::NewGUID() : compIdSave;
			Component* component = GO->AddComponentFromName(compJson["m_name"].get<std::string>(), compId);

			if (component == nullptr)
				continue;

			// Track component for ref in prefab
			objRefs.emplace(compIdSave, component);
		}
	}

	for (json::iterator internalIt = j.begin(); internalIt != j.end(); ++internalIt)
	{
		json& goJson = internalIt.value();

		// Retrieve gameobject
		GameObject* GO = static_cast<GameObject*>(objRefs[HYGUID(internalIt.key())]);

		// Set gameobject parent
		json& goFields = goJson["Internal"];
		std::string parent = goFields["m_parent"].get<std::string>();
		if (parent != "0")
			GO->SetParent(static_cast<GameObject*>(objRefs[HYGUID(parent)]));
		else
			root = GO;

		// First : Create all components of the gameobject
		json& goComponents = goJson["Components"];
		for (json::iterator compIt = goComponents.begin(); compIt != goComponents.end(); ++compIt)
		{
			json& compValues = compIt.value();

			// Retrieve component in prefab
			Component* component = static_cast<Component*>(objRefs[HYGUID(compIt.key())]);

			if (component == nullptr)
				continue;

			// Load saved data
			LoadGameEntity(compValues, objRefs, component->getArchetype(), component);

			component->Initialize();
		}
	}

	return root;
}

void ResourcesLoader::SaveGameObjectRecurse(json& j, const GameObject& gameObject)
{
	// Create global GO container
	json& goJson = j[gameObject.GetUID()];

	// Add internal data
	json& goFields = goJson["Internal"];
	SaveInternalGameObject(goFields, gameObject);

	// Add components data
	json& goComponents = goJson["Components"];
	for (Component* component : gameObject.GetRawComponents())
	{
		json& compJson = goComponents[component->GetUID()];
		SaveGameEntity(compJson, component->getArchetype(), component);
	}

	// Same with children
	for (GameObject* child : gameObject.GetChildren())
		SaveGameObjectRecurse(j, *child);
}

void ResourcesLoader::SaveSceneGraph(json& j, GameObject* root)
{
	for (GameObject* child : root->GetChildren())
		SaveGameObjectRecurse(j, *child);
}

GameObject* ResourcesLoader::LoadSceneGraph(json& j, Scene& scene)
{
	return LoadGameObjects(j, scene, false);
}

#pragma endregion


void ResourcesLoader::SavePrefab(Prefab& prefab, GameObject& gameObject)
{
	// Remove parent to serialize first object has a root
	// Avoid bugs when creating prefab from non-root gameObject
	GameObject* goParent = gameObject.GetParent();

	if (goParent)
		gameObject.SetParent(nullptr);

	SaveGameObjectRecurse(prefab.GetJsonGraph(), gameObject);

	// Reset game object parent
	if (goParent)
		gameObject.SetParent(goParent);
}

GameObject* ResourcesLoader::LoadPrefab(Prefab& prefab, Scene& scene)
{
	return LoadGameObjects(prefab.GetJsonGraph(), scene, true);
}


ExtensionType ResourcesLoader::GetExtensionType(const std::string& extension)
{
	if (ExtensionTypeMap.find(extension) == ExtensionTypeMap.end())
		return ExtensionType::UNKNOWN;

	return ExtensionTypeMap.at(extension);
}

const std::unordered_map<std::string, ExtensionType> ResourcesLoader::ExtensionTypeMap =
{
	{".meta", ExtensionType::META},

	{".texture", ExtensionType::RESOURCE},
	{".model", ExtensionType::RESOURCE},
	{".mesh", ExtensionType::RESOURCE},
	{".skmesh", ExtensionType::RESOURCE},
	{".mat", ExtensionType::RESOURCE},
	{".scene", ExtensionType::RESOURCE},
	{".prefab", ExtensionType::RESOURCE},
	{".skybox", ExtensionType::RESOURCE},
	{".script", ExtensionType::RESOURCE},
	{".terrain", ExtensionType::RESOURCE},
	{".sound", ExtensionType::RESOURCE},

	{ ".fbx", ExtensionType::MODEL },
	{ ".obj", ExtensionType::MODEL },
	{ ".gltf", ExtensionType::MODEL },
	{ ".glb", ExtensionType::MODEL },

	{ ".png", ExtensionType::TEXTURE },
	{ ".jpg", ExtensionType::TEXTURE },
	{ ".jpeg", ExtensionType::TEXTURE },
	{ ".tiff", ExtensionType::TEXTURE },
	{ ".ico", ExtensionType::TEXTURE },

	{ ".wav", ExtensionType::SOUND },
	{ ".ogg", ExtensionType::SOUND },
	{ ".mp3", ExtensionType::SOUND },
	{ ".mp4", ExtensionType::SOUND },

	{ "", ExtensionType::IGNORE },
};

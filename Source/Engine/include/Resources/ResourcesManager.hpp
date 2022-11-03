#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <type_traits>

#include "Resources/Resource/Resource.hpp"
#include "Tools/StringHelper.hpp"
#include "Core/ResourcesTaskPool.hpp"

/**
@brief Resources Manager handle resource creation and storage
*/
class ResourcesManager
{
private:
	std::unordered_map<HYGUID, std::unique_ptr<Resource>> m_resources;

	ResourcesTaskPool m_taskPool;
	std::mutex m_resourceMutex;
	
public:
	ResourcesManager();
	~ResourcesManager();

	/**
	@brief Create resources files of a given resource (.(resourceName) and .meta)

	@param resource : Resource to create files for
	*/
	ENGINE_API void CreateResourceFiles(Resource* resource);

	/**
	@brief Destroy a resource by its UID.

	@param id : Unique identifier
	*/
	ENGINE_API void DestroyResource(const HYGUID& id);


public:
	/**
	@brief Initialize ResourcesManager by importing already-known resources
	*/
	void Initialize();

	/**
	@brief Create a new empty Resource of type R with a new GUID

	@tparam <TResource> : Resource derived class
	@return R* : pointer to the Resource derived class created
	*/
	template <DerivedResource TResource>
	TResource* CreateResource();

	/**
	@brief Create a new empty Resource of type R with a new GUID and create files associated

	@param <TResource> : Resource derived class
	@param path : Path where the resource is stored

	@return R* : pointer to the Resource derived class created
	*/
	template <DerivedResource TResource>
	TResource* CreateResource(const std::string& path);

	/**
	@brief Create a new empty Resource of typeString with an existing GUID. Engine-Only meant for Deserialization.

	@param typeString : const string&
	@param id : Unique identifier
	@return R* : pointer to the Resource derived class created
	*/
	Resource* CreateResource(const std::string& typeString, const HYGUID& id);

	// Delete Resource functions
	// -------------------------

	/**
	@brief Delete a resource by its UID.

	@param id : Unique identifier
	*/
	ENGINE_API void DeleteResource(const HYGUID& id);

	/**
	@brief Delete a resource.

	@param id : Unique identifier
	*/
	ENGINE_API void DeleteResource(Resource* resource);



	// Has Resource functions
	// ----------------------

	/**
	@brief Tells if a resource exists by its UID.

	@param id : Unique identifier
	@return bool : true if resource exists, false if not
	*/
	ENGINE_API bool HasResource(const HYGUID& id);

	/**
	@brief Tells if a resource exists by its path.

	@param path : const std::string&
	@return bool : true if resource exists, false if not
	*/
	ENGINE_API bool HasResourceByPath(const std::string& path);

	/**
	@brief Tells if a resource exists by its name.

	@param name : const std::string&
	@return bool : true if resource exists, false if not
	*/
	ENGINE_API bool HasResourceByFilename(const std::string& name);


	// Load Resource functions
	// -----------------------

	/**
	@brief Load a resource from its path. Must specify the expected Resource type.

	@tparam <TResource> : Resource derived class
	@param path : the file path of the resource
	@return DerivedResourceType : Resource pointer, nullptr if load failed
	*/
	template <DerivedResource TResource>
	void ImportResource(const std::string& path);

	/**
	@brief Load a resource, recognizing its type with its extension

	@param path : the file path of the resource
	@return Resource* : Resource pointer, nullptr if load failed
	*/
	ENGINE_API void ImportNewResource(const std::string& path);


	// Get Resource functions
	// ----------------------

	/**
	@brief Get a resource by its UID. Must specify the expected Resource type.

	@tparam <TResource> : Resource derived class
	@param id : HYGUID
	@return DerivedResourceType : Resource pointer, nullptr if resource not found
	*/
	template <DerivedResource TResource>
	TResource* GetResource(const HYGUID& id);

	/**
	@brief Get a resource by its UID.

	@param id : HYGUID
	@return Resource* : Resource pointer, nullptr if resource not found
	*/
	ENGINE_API Resource* GetResource(const HYGUID& id);

	/**
	@brief Get all resources vector.
	
	@return Resource* : Resource pointer, nullptr if resource not found
	*/
	ENGINE_API std::unordered_map<HYGUID, std::unique_ptr<Resource>>& GetResources();

	/**
	@brief Get a resource by its path.

	@param path : const std::string&
	@return Resource* : Resource pointer, nullptr if resource not found
	*/
	ENGINE_API Resource* GetResourceByPath(const std::string& path);

	/**
	@brief Get a resource by its name.

	@param name : const std::string&
	@return Resource* : Resource pointer, nullptr if resource not found
	*/
	ENGINE_API Resource* GetResourceByFilename(const std::string& name);


	// Get Resources List functions
	// ----------------------------

	/**
	@brief Get the list of all the resources of the desired type
	*/
	ENGINE_API std::vector<Resource*> GetResourcesList(RESOURCE_TYPE type);

	/**
	@brief Get the list of all the names of the ressources of the desired type
	*/
	ENGINE_API std::vector<std::string> GetResourcesNamesList(RESOURCE_TYPE type);

	ResourcesTaskPool& GetTaskPool();
};

#include "Resources/ResourcesManager.inl"
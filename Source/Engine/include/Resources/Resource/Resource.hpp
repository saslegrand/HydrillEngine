#pragma once

#include <string>
#include <Refureku/Object.h>
#include <Refureku/Properties/Instantiator.h>

#include "ECS/SceneObject.hpp"
#include "Resources/ResourceType.hpp"
#include "Types/GUID.hpp"
#include "Types/Serializable.hpp"

#include "Generated/Resource.rfkh.h"

class ResourcesLoader;

class HY_CLASS() ENGINE_API Resource : public SceneObject, public Serializable
{
protected:
// 'std::string' : dll-export can clash if projects don't have same version of std library, but in this case we know it is good
#pragma warning(disable:4251) 
	std::string		m_selfName = "";	/** @brief Name of the resource class (from reflection) */
	std::string		m_filename = "";	/** @brief Filename of the resource (name + extension) */
	std::string		m_filepath = "";	/** @brief Filepath of the resource (root : Assets folder) */
#pragma warning(default:4251)

	RESOURCE_TYPE	m_type = RESOURCE_TYPE::UNKNOWN;

public:
	Resource(const HYGUID& uid, const std::string& name);

public:
	virtual ~Resource();

	/**
	@brief Copy all members of a target resource, for duplication

	@param resource : target to copy
	*/
	virtual void Copy(const Resource& resource);

	/**
	@brief Rename the resource and its associated files

	@param resource : target to copy
	*/
	virtual void Rename(std::string_view newName);

	/**
	@brief Get the type of the resource

	@return RESOURCE_TYPE : type
	*/
	RESOURCE_TYPE GetType() const;

	/**
	@brief Set file access information from current path (filePath, filename)

	@return path : Path of the resource
	*/
	void SetFileInfo(const std::string& path);

	/**
	@brief Set the type of the resource

	@param type : type of the resource
	*/
	void SetType(RESOURCE_TYPE type);

	/**
	@brief Get the name of the resource

	@return string : name of the resource
	*/
	const std::string& GetClassName() const;

	/**
	@brief Get the name of the resource

	@return string : name of the resource
	*/
	const std::string& GetFilename() const;

	/**
	@brief Set the name or rename the resource

	@param name : name of the resource
	*/
	void SetFilename(const std::string& filename);

	/**
	@brief Get the filepath of the resource

	@return string : filepath of the resource
	*/
	const std::string& GetFilepath() const;

	/**
	@brief Set the filepath of the resource

	@param path : filepath of the resource
	*/
	void SetFilepath(const std::string& path);

	/**
	@brief Import the resource in the engine

	@param path : path of the resource
	*/
	virtual bool Import(const std::string& path);

	/**
	@brief Loads the resource on GPU
	*/
	virtual void LoadInGPUMemory() {}

	/**
	@brief Unloads the resource from GPU
	*/
	virtual void UnloadFromGPUMemory() {}

	Resource_GENERATED
};

template <typename TResource>
concept DerivedResource = std::is_base_of_v<Resource, TResource>;

File_Resource_GENERATED

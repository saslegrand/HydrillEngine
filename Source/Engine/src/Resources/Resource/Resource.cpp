#include <nlohmann/json.hpp>

#include "Tools/StringHelper.hpp"
#include "Resources/Loaders/ResourcesLoader.hpp"

#include "Generated/Resource.rfks.h"

Resource::Resource(const HYGUID& uid, const std::string& selfName)
	: SceneObject(selfName, uid), m_selfName(selfName)
{
}

//Resource::Resource(const Resource& resource)
//{
//	m_UID = resource.GetUID();
//	Copy(resource);
//}

Resource::~Resource()
{
}

void Resource::Copy(const Resource& resource)
{
	m_name = resource.m_name;
	m_filepath = resource.m_filepath;
	m_filename = resource.m_filename;
	m_type = resource.m_type;
}

void Resource::Rename(std::string_view newName)
{
	StringHelper::RenameFile(m_filepath, newName);
	StringHelper::RenameFile(m_filepath + ".meta", std::string(newName) + ".meta");

	std::string newPath = StringHelper::GetDirectory(m_filepath) + std::string(newName);
	SetFileInfo(newPath);
}

RESOURCE_TYPE Resource::GetType() const
{
	return m_type;
}

void Resource::SetType(RESOURCE_TYPE type)
{
	m_type = type;
}

void Resource::SetFileInfo(const std::string& path)
{
	SetFilepath(path);
	SetFilename(StringHelper::GetFileNameFromPath(m_filepath));
	m_name = StringHelper::GetFileNameWithoutExtension(m_filename);
}

const std::string& Resource::GetClassName() const
{
	return m_selfName;
}

const std::string& Resource::GetFilename() const
{
	return m_filename;
}

void Resource::SetFilename(const std::string& filename)
{
	m_filename = filename;
}

const std::string& Resource::GetFilepath() const
{
	return m_filepath;
}

void Resource::SetFilepath(const std::string& path)
{
	m_filepath = path;
}

bool Resource::Import(const std::string& path)
{
	SetFileInfo(path);
	return true;
}
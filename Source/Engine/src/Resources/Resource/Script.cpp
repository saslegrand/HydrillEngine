
#include <nlohmann/json.hpp>

#include "Generated/Script.rfks.h"

rfk::UniquePtr<Script> Script::defaultInstantiator(const HYGUID& uid)
{
	return rfk::makeUnique<Script>(uid);
}

Script::Script(const HYGUID& uid)
	: Resource(uid, staticGetArchetype().getName())
{
	m_type = RESOURCE_TYPE::SCRIPT;
}

bool Script::Import(const std::string& path)
{
	return Resource::Import(path + ".script");
}

void Script::Serialize()
{
	json j;

	WriteJson(m_filepath, j);
}
#include "Types/Serializable.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

#include "Core/Logger.hpp"

void Serializable::ReadJson(const std::string& path, json& j)
{
	std::ifstream i(path);
	if (!i.is_open())
	{
		Logger::Warning("Can't read asset file : " + path);
		return;
	}

	j = json::parse(i);
	i.close();
}

void Serializable::WriteJson(const std::string& path, json& j)
{
	std::ofstream o(path);
	if (!o)
	{
		Logger::Warning("Cannot write asset file " + path);
		return;
	}

	o << std::setw(1) << j << std::endl;
	o.close();
}
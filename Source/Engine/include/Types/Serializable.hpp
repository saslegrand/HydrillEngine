#pragma once

#include <nlohmann/json_fwd.hpp>
using json = nlohmann::json;

#include "EngineDLL.hpp"

/**
@brief Serializable makes the class usable for file serialization
*/
class ENGINE_API Serializable
{
protected:
	/**
	@brief Open a file and extract json structure from it

	@param const std::string& : path
	@param json& : j
	*/
	void ReadJson(const std::string& path, json& j);

	/**
	@brief Write a json structure inside a file

	@param const std::string& : path
	@param json& : j
	*/
	void WriteJson(const std::string& path, json& j);

public:

	/**
	@brief Serialize object, meaning writing data into a file
	*/
	virtual void Serialize() {}

	/**
	@brief Deserialize object, meaning retreiving data from a file
	*/
	virtual void Deserialize() {}
};
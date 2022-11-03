#pragma once

#include <map>

#include "Maths/Matrix3.hpp"
#include "Maths/Vector4.hpp"
#include "Maths/Quaternion.hpp"
#include "Physics/BoolVector3.hpp"
#include "Types/Color.hpp"
#include "Types/Color.hpp"
#include "Core/BaseObject.hpp"

#include "Tools/ReflectedSTD.hpp"
#include "Tools/SerializationUtils.hpp"

#include "Core/Logger.hpp"

/**
@brief Util struct to save entity in json file
*/
struct RFKJsonLoadSaveData
{
	nlohmann::json& container;
	BaseObject* object;
};

/**
@brief Util struct to map reflected class archetype id with specific save functions
*/
struct RFKJsonSaveMap
{
private:
	static const std::map<size_t, void (*)(nlohmann::json& jsonFile, void* fieldData)> m_fieldSetMap;

	/**
	@brief Set a unique value object in a json file (scalar, string)

	@param field : Field to save
	@param data : Field owner and json data
	*/
	template <typename Type>
	static void SetUniqueValue(nlohmann::json& jsonFile, void* fieldData);

	/**
	@brief Set a unique value object in a json file (scalar, string)

	@param field : Field to save
	@param data : Field owner and json data
	*/
	template <typename Type>
	static void SetSTDValue(nlohmann::json& jsonFile, void* fieldData);

	/**
	@brief Set a multi value object in a json file (Vectors, Matrices)

	@param field : Field to save
	@param data : Field owner and json data
	*/
	template <typename Type, typename ContentType>
	static void SetMultiValue(nlohmann::json& jsonFile, void* fieldData);

public:
	/**
	@brief Set a field value object in a json file fro field archetype

	@param field : Field to save
	@param data : Field owner and json data
	*/
	inline static bool SetValueFromType(nlohmann::json& jsonFile, rfk::Archetype const* fieldArch, void* fieldData);
};



struct RFKJsonLoadMap
{
private:
	static const std::map<size_t, void (*)(nlohmann::json& jsonFile, void* fieldData)> m_fieldSetMap;

	/**
	@brief Set a unique value object in a json file (scalar, string)

	@param field : Field to save
	@param data : Field owner and json data
	*/
	template <typename Type>
	static void LoadUniqueValue(nlohmann::json& jsonFile, void* fieldData);

	/**
	@brief Set a multi value object in a json file (Vectors, Matrices)

	@param field : Field to save
	@param data : Field owner and json data
	*/
	template <typename Type, typename ContentType>
	static void LoadMultiValue(nlohmann::json& jsonFile, void* fieldData);

public:
	/**
	@brief Set a field value object in a json file for field archetype

	@param field : Field to save
	@param data : Field owner and json data
	*/
	inline static bool LoadValueFromType(nlohmann::json& jsonFile, rfk::Archetype const* fieldArch, void* fieldData);
};

#include "Tools/RFKUtils.inl"
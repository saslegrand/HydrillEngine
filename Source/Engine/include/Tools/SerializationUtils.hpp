#pragma once
#include <vector>

#include "nlohmann/json.hpp"
#include "EngineContext.hpp"
#include "Resources/ResourcesManager.hpp"

namespace Serialization
{
	/**
	@brief Try get a value from a json given a json field name

	@param j : parent json field
	@param field : field we try to retrieve the resource from
	@param out : value reference to fill if the field was found

	@param true if value was successfully filled
	*/
	template<typename Type>
	bool TryGetValue(nlohmann::json& j, Type& out, nlohmann::detail::value_t valueType)
	{
		if (!j.is_null())
		{
			//	Here arrays are not accepted
			if (j.type() == nlohmann::detail::value_t::array) return false;
			if (j.type() != valueType) return false;

			out = j.get<Type>();

			return true;
		}
		
		// The value contain is null
		return false;
	}

	/**
	@brief Try get a value from a json given a json field name

	@param j : parent json field
	@param field : field we try to retrieve the resource from
	@param out : value reference to fill if the field was found

	@param true if value was successfully filled
	*/
	template<typename Type>
	bool TryGetValue(nlohmann::json& j, std::string_view key, Type& out)
	{
		//	Find the field
		auto it = j.find(key);

		//	If the field exist, then get the value and return true
		if (it != j.end())
		{
			json& jsonIt = *it;
			//	Here arrays are not accepted
			if (jsonIt.type() == nlohmann::detail::value_t::array) return false;

			out = jsonIt.get<Type>();

			return true;
		}

		//	Else if failed to find thre resource, return false
		return false;
	}

	/**
	@brief Try get a resource from a json given a json field name

	@param j : parent json field
	@param field : field we try to retrieve the resource from
	@param out : resource pointer to fill if the field was found

	@param true if resource was successfully filled
	*/
	template <DerivedResource TResource>
	bool TryGetResource(nlohmann::json& j, std::string_view field, TResource*& out)
	{
		std::string guid;

		//	Try get the guid string
		if (TryGetValue<std::string>(j, field, guid))
		{
			//	Try get the resource from the resources manager
			out = EngineContext::Instance().resourcesManager->GetResource<TResource>(HYGUID(guid));
			return true;
		}

		//	Else if failed to find thre resource, set the resource to null then return false
		out = nullptr;
		return false;
	}

	/**
	@brief Set a container given a value in a json field

	@param j : json field to fill
	@param value : value to save in json
	*/
	template <typename ContentType, typename Type>
	void SetContainer(nlohmann::json& j, Type& value)
	{
		// Get field value from type and get first element pointer
		ContentType* valuePtr = reinterpret_cast<ContentType*>(&value);

		// Get element size
		uint32_t count = sizeof(Type) / sizeof(ContentType);

		std::vector<ContentType> vec;
		vec.resize(count);

		// Set field value in vector
		for (uint32_t i = 0; i < count; i++)
			vec[i] = valuePtr[i];

		// Add vector to json
		j = vec;
	}

	/**
	@brief Try to load a container from a json field and return true wether the field was found or not

	@param j : json parent field
	@param key : field name
	@param out : container to fill from the field

	@return True if the field was found in the parent json field and the container would be successfully filled
	*/
	template <typename ContentType, typename Type>
	bool TryGetContainer(nlohmann::json& j, std::string_view key, Type& out)
	{
		//	Find the field
		auto it = j.find(key);
		
		//	If the field exist, then get the value and return true
		if (it != j.end())
		{
			// Get json content
			std::vector<ContentType> jsonContent = it.value().get<std::vector<ContentType>>();

			// Get value pointer on first element
			ContentType* valuePtr = reinterpret_cast<ContentType*>(&out);

			// Fill new field value
			for (size_t i = 0; i < jsonContent.size(); i++)
				valuePtr[i] = jsonContent[i];

			return true;
		}

		return false;
	}

	/**
	@brief load a container from a json field that is valid

	@param j : a VALID json field
	*/
	template <typename Type, typename ContentType>
	Type LoadContainer(nlohmann::json& j)
	{
		// Create new field value
		Type typedValue;

		// Get json content
		std::vector<ContentType> jsonContent = j.get<std::vector<ContentType>>();

		// Get value pointer on first element
		ContentType* valuePtr = reinterpret_cast<ContentType*>(&typedValue);

		// Fill new field value
		for (size_t i = 0; i < jsonContent.size(); i++)
			valuePtr[i] = jsonContent[i];

		return typedValue;
	}
}
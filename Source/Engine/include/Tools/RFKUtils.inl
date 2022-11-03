
#pragma region JSON_SAVE

template <typename Type>
void RFKJsonSaveMap::SetUniqueValue(nlohmann::json& jsonFile, void* fieldData)
{
	Type& typedData = *static_cast<Type*>(fieldData);
	jsonFile = typedData;
}

template <typename Type>
void RFKJsonSaveMap::SetSTDValue(nlohmann::json& jsonFile, void* fieldData)
{
	Type& typedData = *static_cast<Type*>(fieldData);
	jsonFile = typedData.data();
}

template <typename Type, typename ContentType>
void RFKJsonSaveMap::SetMultiValue(nlohmann::json& jsonFile, void* fieldData)
{
	Type& typedData = *static_cast<Type*>(fieldData);
	Serialization::SetContainer<ContentType>(jsonFile, typedData);
}


inline bool RFKJsonSaveMap::SetValueFromType(nlohmann::json& jsonFile, rfk::Archetype const* fieldArch, void* fieldData)
{
	auto it = m_fieldSetMap.find(fieldArch->getId());

	if (it == m_fieldSetMap.end())
		return false;

	it->second(jsonFile, fieldData);

	return true;
}
#pragma endregion


#pragma region JSON_LOAD

template <typename Type>
static void RFKJsonLoadMap::LoadUniqueValue(nlohmann::json& jsonFile, void* fieldData)
{
	*static_cast<Type*>(fieldData) = jsonFile.get<Type>();
}


template <typename Type, typename ContentType>
static void RFKJsonLoadMap::LoadMultiValue(nlohmann::json& jsonFile, void* fieldData)
{
	Type typedValue = Serialization::LoadContainer<Type, ContentType>(jsonFile);

	*static_cast<Type*>(fieldData) = typedValue;
}


inline bool RFKJsonLoadMap::LoadValueFromType(nlohmann::json& jsonFile, rfk::Archetype const* fieldArch, void* fieldData)
{
	auto it = m_fieldSetMap.find(fieldArch->getId());

	if (it == m_fieldSetMap.end())
		return false;

	it->second(jsonFile, fieldData);

	return true;
}

#pragma endregion
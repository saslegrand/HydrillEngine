#include "Tools/RFKUtils.hpp"

inline std::map<size_t, void (*)(nlohmann::json& jsonFile, void* fieldData)> const RFKJsonSaveMap::m_fieldSetMap {

		{ rfk::getType<bool>().getArchetype()->getId(),				&RFKJsonSaveMap::SetUniqueValue<bool> },
		{ rfk::getType<char>().getArchetype()->getId(),				&RFKJsonSaveMap::SetUniqueValue<char> },
		{ rfk::getType<short>().getArchetype()->getId(),			&RFKJsonSaveMap::SetUniqueValue<short> },
		{ rfk::getType<unsigned short>().getArchetype()->getId(),	&RFKJsonSaveMap::SetUniqueValue<unsigned short> },
		{ rfk::getType<int>().getArchetype()->getId(),				&RFKJsonSaveMap::SetUniqueValue<int> },
		{ rfk::getType<unsigned int>().getArchetype()->getId(),		&RFKJsonSaveMap::SetUniqueValue<unsigned int> },
		{ rfk::getType<long>().getArchetype()->getId(),				&RFKJsonSaveMap::SetUniqueValue<long> },
		{ rfk::getType<unsigned long>().getArchetype()->getId(),	&RFKJsonSaveMap::SetUniqueValue<unsigned long> },
		{ rfk::getType<float>().getArchetype()->getId(),			&RFKJsonSaveMap::SetUniqueValue<float> },
		{ rfk::getType<double>().getArchetype()->getId(),			&RFKJsonSaveMap::SetUniqueValue<double> },

		{ rfk::getType<HyString>().getArchetype()->getId(),			&RFKJsonSaveMap::SetSTDValue<HyString> },
		{ rfk::getType<BoolVector3>().getArchetype()->getId(),		&RFKJsonSaveMap::SetMultiValue<BoolVector3, bool> },
		{ rfk::getType<Color3>().getArchetype()->getId(),			&RFKJsonSaveMap::SetMultiValue<Color3, float> },
		{ rfk::getType<Color4>().getArchetype()->getId(),			&RFKJsonSaveMap::SetMultiValue<Color4, float> },
		{ rfk::getType<Vector2>().getArchetype()->getId(),			&RFKJsonSaveMap::SetMultiValue<Vector2, float> },
		{ rfk::getType<Vector3>().getArchetype()->getId(),			&RFKJsonSaveMap::SetMultiValue<Vector3, float> },
		{ rfk::getType<Vector4>().getArchetype()->getId(),			&RFKJsonSaveMap::SetMultiValue<Vector4, float> },
		{ rfk::getType<Quaternion>().getArchetype()->getId(),		&RFKJsonSaveMap::SetMultiValue<Quaternion, float> },
		{ rfk::getType<Matrix3>().getArchetype()->getId(),			&RFKJsonSaveMap::SetMultiValue<Matrix3, float> },
		{ rfk::getType<Matrix4>().getArchetype()->getId(),			&RFKJsonSaveMap::SetMultiValue<Matrix4, float> }
};

inline std::map<size_t, void (*)(nlohmann::json& jsonFile, void* fieldData)> const RFKJsonLoadMap::m_fieldSetMap{

		{ rfk::getType<bool>().getArchetype()->getId(),				&RFKJsonLoadMap::LoadUniqueValue<bool> },
		{ rfk::getType<char>().getArchetype()->getId(),				&RFKJsonLoadMap::LoadUniqueValue<char> },
		{ rfk::getType<short>().getArchetype()->getId(),			&RFKJsonLoadMap::LoadUniqueValue<short> },
		{ rfk::getType<unsigned short>().getArchetype()->getId(),	&RFKJsonLoadMap::LoadUniqueValue<unsigned short> },
		{ rfk::getType<int>().getArchetype()->getId(),				&RFKJsonLoadMap::LoadUniqueValue<int> },
		{ rfk::getType<unsigned int>().getArchetype()->getId(),		&RFKJsonLoadMap::LoadUniqueValue<unsigned int> },
		{ rfk::getType<long>().getArchetype()->getId(),				&RFKJsonLoadMap::LoadUniqueValue<long> },
		{ rfk::getType<unsigned long>().getArchetype()->getId(),	&RFKJsonLoadMap::LoadUniqueValue<unsigned long> },
		{ rfk::getType<float>().getArchetype()->getId(),			&RFKJsonLoadMap::LoadUniqueValue<float> },
		{ rfk::getType<double>().getArchetype()->getId(),			&RFKJsonLoadMap::LoadUniqueValue<double> },

		{ rfk::getType<HyString>().getArchetype()->getId(),			&RFKJsonLoadMap::LoadUniqueValue<HyString> },
		{ rfk::getType<BoolVector3>().getArchetype()->getId(),		&RFKJsonLoadMap::LoadMultiValue<BoolVector3, bool> },
		{ rfk::getType<Color3>().getArchetype()->getId(),			&RFKJsonLoadMap::LoadMultiValue<Color3, float> },
		{ rfk::getType<Color4>().getArchetype()->getId(),			&RFKJsonLoadMap::LoadMultiValue<Color4, float> },
		{ rfk::getType<Vector2>().getArchetype()->getId(),			&RFKJsonLoadMap::LoadMultiValue<Vector2, float> },
		{ rfk::getType<Vector3>().getArchetype()->getId(),			&RFKJsonLoadMap::LoadMultiValue<Vector3, float> },
		{ rfk::getType<Vector4>().getArchetype()->getId(),			&RFKJsonLoadMap::LoadMultiValue<Vector4, float> },
		{ rfk::getType<Quaternion>().getArchetype()->getId(),		&RFKJsonLoadMap::LoadMultiValue<Quaternion, float> },
		{ rfk::getType<Matrix3>().getArchetype()->getId(),			&RFKJsonLoadMap::LoadMultiValue<Matrix3, float> },
		{ rfk::getType<Matrix4>().getArchetype()->getId(),			&RFKJsonLoadMap::LoadMultiValue<Matrix4, float> }
};
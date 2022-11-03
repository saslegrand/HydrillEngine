#pragma once

#include <map>
#include <string_view>
#include <limits>

#include <Refureku/TypeInfo/Variables/Field.h>
#include <Refureku/TypeInfo/Entity/Entity.h>

#include "Tools/InspectorUtils.hpp"

//	Forward declarations

class	Window;
class	Resource;
class	GameObject;
class	BaseObject;
class	Component;
class	Mesh;

struct	HyString;
struct	Color3;
struct	Color4;
struct	ImVec2;
struct	Vector2;
struct	Vector3;
struct	Vector4;
struct	Quaternion;
struct	Matrix3;
struct	Matrix4;
struct	BoolVector3;

namespace GUI
{
	struct InspectFieldData
	{
		rfk::Class const* ownerClass;
		BaseObject* ownerObject;

		std::string_view name;
		rfk::Archetype const* arch;
		void* data;
		bool isPointer;
	};

	struct InspectPropertiesData
	{
		double dragMin = std::numeric_limits<double>::lowest();
		double dragMax = (std::numeric_limits<double>::max)();
		bool readOnly = false;

		//TestProperty const* onModify;
	};

	enum class EFieldInspectType
	{
		FUNDAMENTAL,
		ENUM,
		FLAG,
		REFERENCE
	};

	/**
	@brief This function launch an Imgui context linked with a GLFW window

	@param window : existing window class pointer
	*/
	void LaunchGUI(Window* window);

	/**
	@brief Shut down ImGui system
	*/
	void ShutDownGUI();

	/**
	@brief Must be called at the beginning of each frame
	*/
	void NewFrameGUI();

	/**
	@brief Render all GUI which were called between NewFrameGUI and RenderGUI
	*/
	void RenderGUI();


	/**
	@brief Function overlay of ImGui::PushStyleColor

	@param target : color to change
	@param color  : new color to assign
	*/
	void PushColor(const int target, const Color4& color);

	/**
	@brief Function overlay of ImGui::PopStyleColor

	@param count : number of color to pop 
	*/
	void PopColor(const int count = 1);


	/**
	@brief Function overlay of ImGui::PopStyleVar

	@param target : style to change 
	@param value : value to assign
	*/
	void PushStyle(const int target, const float value);

	/**
	@brief Function overlay of ImGui::PopStyleVar.

	@param target : style to change 
	@param value : value to assign
	*/
	void PushStyle(const int target, const ImVec2& value);

	/**
	@brief Function overlay of ImGui::PopStyleVar

	@param count : number of style to pop
	*/
	void PopStyle(const int count = 1);

	/**
	@brief Display a tooltip after a given delay hovering an item

	@param text : tooltip text
	@param delay : delay before showing the tooltip
	*/
	void DelayedTooltip(const char* text, float delay = 0.5f);

	/**
	@brief Cast a texture ID to void* 

	@param textureID : GL texture ID to convert to ImGUI texture ID

	@return ImGUI Texture ID type
	*/
	void* CastTextureID(const unsigned int textureID);

	/**
	@brief Display a colored float slider

	@param text : Name of the value attached
	@param ID : Drag ID
	@param out_value : Attached value
	@param color : Border color

	@return boolean wether the widget was used or not
	*/
	bool ColoredFloatDrag(const char* ID, float& outValue, const Color4& color, GUI::InspectPropertiesData const& propertiesData);

	void FieldLabel(std::string_view label);
	bool BeginField(const char* ID);
	void BeginFieldContent();
	void EndField();

	/**
	Base function to display a field in the editor inspector

	@param fieldData : Field info
	@param properties : Properties attached to the field
	@param fieldType : Type of the field (fundamental, enum, reference)

	@return bool : True if the field is modified, false otherwise
	*/
	template <typename Type>
	bool DrawField(InspectFieldData& fieldData, PropertyContainer const& properties, GUI::EFieldInspectType fieldType);

	/**
	Base function to display an enum in the editor inspector

	@param fieldName : Name of the field
	@param fieldArch : Archetype of the field
	@param fieldData : Ptr on the field data
	@param propertiesData : Properties attached to the field

	@return bool : True if the field is modified, false otherwise
	*/
	bool DrawEnum(const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, GUI::InspectPropertiesData const& propertiesData);

	/**
	Base function to display a flag in the editor inspector

	@param fieldName : Name of the field
	@param fieldArch : Archetype of the field
	@param fieldData : Ptr on the field data
	@param propertiesData : Properties attached to the field

	@return bool : True if the field is modified, false otherwise
	*/
	bool DrawFlag(const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, GUI::InspectPropertiesData const& propertiesData);

	/**
	Base function to display a SceneObject in the editor inspector

	@param fieldName : Name of the field
	@param fieldArch : Archetype of the field
	@param fieldData : Ptr on the field data
	@param propertiesData : Properties attached to the field

	@return bool : True if the field is modified, false otherwise
	*/
	template <typename Type>
	bool DrawRef(const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, GUI::InspectPropertiesData const& propertiesData);
	template <>
	bool DrawRef<Resource>(const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, GUI::InspectPropertiesData const& propertiesData);
	template <>
	bool DrawRef<GameObject>(const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, GUI::InspectPropertiesData const& propertiesData);
	template <>
	bool DrawRef<Component>(const char* fieldName, rfk::Archetype const* fieldArch, void* fieldData, GUI::InspectPropertiesData const& propertiesData);


	/**
	Base function to display a fundamental field in the editor inspector

	@param fieldName : Name of the field
	@param fieldData : Ptr on the field data
	@param propertiesData : Properties attached to the field

	@return bool : True if the field is modified, false otherwise
	*/
	template <typename Type>
	bool DrawFundamental(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<bool>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<char>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<short>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<unsigned short>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<int>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<unsigned int>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<long long>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<unsigned long long>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<float>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<double>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<HyString>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<Color3>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<Color4>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<BoolVector3>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<Vector2>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<Vector3>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<Vector4>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<Quaternion>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<Matrix3>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
	template <>
	bool DrawFundamental<Matrix4>(const char* fieldName, void* fieldData, InspectPropertiesData const& propertiesData);
}

namespace rfk
{
	class Field;
}

class Component;

struct RFKInspectObject
{
private:
	static const std::map<size_t, bool (*)(GUI::InspectFieldData& fieldData, PropertyContainer const& properties, GUI::EFieldInspectType fieldType)> m_inspectMap;

public:
	/**
	@brief Set a field value object in a json file fro field archetype

	@param field : Field to save
	@param data : Field owner and json data
	@param isModified : True if the value modified
	*/
	static bool TryInspectFundamental(GUI::InspectFieldData& fieldData, PropertyContainer const& properties, bool& isModified);
	static bool TryInspectEnum(GUI::InspectFieldData& fieldData, PropertyContainer const& properties, bool& isModified);
	static bool TryDisplayEntity(GUI::InspectFieldData& fieldData, rfk::Class const* fieldClass, PropertyContainer const& properties);
	static bool TryInspectReference(GUI::InspectFieldData& fieldData, rfk::Class const* fieldClass, PropertyContainer const& properties, bool& isModified);
};
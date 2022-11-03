#pragma once

#include <string_view>
#include <unordered_map>

#include <Refureku/Refureku.h>

namespace GUI
{
	struct InspectFieldData;
}

using PropertyContainer = std::unordered_map<size_t, rfk::Property const*>;

class BaseObject;

struct InspectorMethods
{
	/**
	This function allow to display all the relfected members of a given object.
	The goal is to iterate through the object fields and setup the attached properties.

	@param object : BaseObject to display
	@param rfkClass : BaseObject archetype

	@return bool : True the object has been modified, false otherwise
	*/
	static bool DisplayEntity(BaseObject* object, rfk::Class const* rfkClass);

	/**
	This function allow to display the relfected non-template members of a given object.
	The goal is to draw correctly the member given its type (float, Vector3, Scene*, ...)

	@param fieldData : Reflection data of the current object member
	@param properties : Properties attached to the object member

	@return bool : True the object has been modified, false otherwise
	*/
	static bool DisplayObject(GUI::InspectFieldData& fieldData, PropertyContainer& properties);

	/**
	This function allow to display the relfected template members of a given object.
	The goal is to draw correctly the member given its type (HyVector, Flags)

	@param fieldData : Reflection data of the current object member
	@param cTempInstance : Archetype of the object member
	@param properties : Properties attached to the object member

	@return bool : True the object has been modified, false otherwise
	*/
	static bool DisplayTemplateObject(GUI::InspectFieldData& fieldData, rfk::ClassTemplateInstantiation const* cTempInstance, PropertyContainer& properties);
};
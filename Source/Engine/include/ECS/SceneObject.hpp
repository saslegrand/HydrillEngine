#pragma once

#include <string>

#include "EngineDLL.hpp"

#include <Refureku/Object.h>

#include "Tools/ReflectedSTD.hpp"
#include "Tools/RFKProperties.hpp"
#include "Core/BaseObject.hpp"
#include "Types/GUID.hpp"
//#include "Tools/Event.hpp"

#include "Generated/SceneObject.rfkh.h"

/** @brief Base object class for GameObject and Component */
class HY_CLASS() ENGINE_API SceneObject : public BaseObject
{
protected:
	bool m_isActive = true;

	#pragma warning(disable:4251)
	HY_FIELD(PHide()) HyString m_name;
	#pragma warning(default:4251)

	HYGUID m_UID;

public:
	SceneObject() = default;

	/**
	@brief Construct a baseObject with a name

	@param name : New object name
	*/
	SceneObject(const HyString& name, const HYGUID & uid);

public:
	/**
	@brief Activate/Deactivate the object

	@param value : Activate if true, deactivate otherwise
	*/
	virtual void SetActive(bool value);

	/**
	@brief Tell if the object is currently active or not

	@return bool : Current active state of the object
	*/
	virtual bool IsActive() const;

	/**
	@brief Tell if the object itself is currently active or not

	@return bool : Current active state of the object
	*/
	virtual bool IsSelfActive() const;

	/**
	@brief Change the object name

	@param name : new object name
	*/
	void SetName(const std::string & name);

	/**
	@brief Get the name of the object

	@return string : Name of the object
	*/
	const std::string& GetName() const;

	/**
	@brief Get the unique id of the object

	@return HYGUID : Unique id of the object
	*/
	const HYGUID& GetUID() const;

	SceneObject_GENERATED
};

#include "ECS/SceneObject.inl"

File_SceneObject_GENERATED
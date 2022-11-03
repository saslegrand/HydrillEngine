#pragma once

#include <Refureku/Object.h>
#include <Refureku/Properties/Instantiator.h>

#include "EngineDLL.hpp"
#include "ECS/SceneObject.hpp"

#include "Generated/Component.rfkh.h"

class GameObject;

/**
@brief Base class for gameobject component.
Derived from rfk::Object to allow dynamic reflection
*/
class HY_CLASS() ENGINE_API Component : public SceneObject
{
protected:
	/**
	@brief Info to know if the component can be instantiated
	multiple times in the same GameObject
	*/
	bool m_isUnique = false;
	bool m_isDestroyed = false;

public:

	/**
	@brief Owner of the component
	*/
	GameObject& gameObject;

public:
	/**
	@brief Default component constructor.
	Component may always have a gameObject reference else it can't exist

	@param gameObject : Owner of the component
	@param name : Name of the component (archetype)
	@param uid : Unique identifier of the component
	*/
	Component(GameObject& gameObject, const HyString& name, const HYGUID& uid);
	Component(Component const& cam) = delete;
	Component(Component&& cam) = default;

	Component& operator=(Component const& cam) = delete;
	Component& operator=(Component && cam) = default;

	virtual ~Component();

public:

	/**
	@brief First called function after the component was loaded
	*/
	virtual void Initialize() {};

	/**
	@brief Give the information if the component could be added multiple times
	
	@return bool : true can be added multiple times, false can only be added once
	*/
	bool IsUnique() const;

	/**
	Util function call when the component is modified (editor purpose only)
	*/
	virtual void OnModify();

	/**
	Util function call when the component is destroyed (game purpose only)
	*/
	virtual void OnDestroy();

	/**
	Tell if the component is currently waiting for being destroyed
	*/
	bool IsDestroyed() const;

	/**
	Set the component as destroyed (avoid multiple remove task)
	*/
	void SetAsDestroyed();

	/**
	Activate/Deactivate the component

	@param isActive : activate if true, deactivate otherwise
	*/
	void SetActive(bool isActive) override;

	/**
	Tell if the component is currently activated

	@return bool : True if activated, false otherwise
	*/
	bool IsActive() const override;

	Component_GENERATED
};

template <typename Comp>
concept DerivedComponent = std::is_base_of_v<Component, Comp>;

File_Component_GENERATED
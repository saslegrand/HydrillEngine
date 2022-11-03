#pragma once

#include "ECS/GameObject.hpp"

#include "Generated/Behavior.rfkh.h"

/**
@brief Base class for user script behavior
*/
class HY_CLASS() ENGINE_API Behavior : public Component
{
public:
	/**
	@brief Default component constructor.
	Component may always have a gameObject reference else it can't exist
	
	@param gameObject : Owner of the component
	@param name : Name of the component (archetype)
	@param uid : Unique identifier of the component
	*/
	Behavior(GameObject& gameObject, const HyString& name, const HYGUID& uid);
	Behavior(Behavior const& behavior) = delete;
	Behavior(Behavior&& behavior) = default;

	Behavior& operator=(Behavior const& behavior) = delete;
	Behavior& operator=(Behavior && behavior) = default;

	~Behavior();

public:
	/**
	@brief First function to be called after the construction
	*/
	virtual void Awake() {}

	/**
	@brief Function called after the Awake of every components (or directly if instance spawned in game)
	*/
	virtual void Start() {}

	/**
	@brief Main function called every frame
	
	@param tick : frame deltatime
	*/
	virtual void Update(float tick) {}

	/**
	@brief Function called every fixed time
	
	@param fixedTick : fixed deltatime
	*/
	virtual void FixedUpdate(float fixedTick) {}

	/**
	@brief Function called every frame after every components Update
	*/
	virtual void LateUpdate() {}

	/**
	@brief Function called when the component is activated
	*/
	virtual void OnEnable() {}

	/**
	@brief Function called when the component is deactivated
	*/
	virtual void OnDisable() {}

	/**
	@brief Activate/Deactivate the component and call the OnEnable/Disable function
	
	@param value : true for activating the object, false otherwise
	*/
	void SetActive(bool value) override;
	
	Behavior_GENERATED
};

File_Behavior_GENERATED
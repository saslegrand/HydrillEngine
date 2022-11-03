#pragma once

#include <vector>
#include <Refureku/TypeInfo/Archetypes/Struct.h>

#include "Tools/Event.hpp"

class Behavior;
class GameObject;
class HYGUID;

/**
@brief System that handle every behaviors in the game
*/
class BehaviorSystem
{
private:
	std::vector<std::unique_ptr<Behavior>> m_behaviors;

	/**
	@brief Flag to know if the system has to call Awake/Start functions on behavior registering
	*/
	bool m_shouldInitiateBehavior = false;
	bool m_addBehavior = false;

	// Event called when a behavior is added in game (Awake/Start)
	Event<> m_awakeInitialize;
	Event<> m_startInitialize;

	/**
	@brief Invoke and clear initialize events
	*/
	void InitializeNewBehaviors();

public:
	/**
	@brief Call AwakeAll/StartAll and allow Awake/Start call on register
	*/
	ENGINE_API void StartSystem();

	/**
	@brief Not allowed Awake/Start call on register
	*/
	ENGINE_API void StopSystem();

	/**
	@brief Call Awake function for all components (call on play)
	*/
	void AwakeAll();

	/**
	@brief Call Start function for all components (call on play after Awake)
	*/
	void StartAll();

	/**
	@brief Call Update function for all components

	@param tick : Engine deltaTime
	*/
	void UpdateAll(float tick);

	/**
	@brief Call FixedUpdate function for all components
	
	@param fixedTick : Engine fixedDeltaTime
	*/
	void FixedUpdateAll(float fixedTick);

	/**
	@brief Call LateUpdate function for all components
	*/
	void LateUpdateAll();

	/**
	@brief Add a new behavior to the system
	
	@param owner : Owner of the behavior
	@param id : Unique identifier of the behavior

	@return Behavior* : Created behavior, nullptr if not valid
	*/
	Behavior* AddBehaviorInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id);

	/**
	@brief Remove an existing behavior in the system
	
	@param behavior : behavior to remove
	*/
	void RemoveBehaviorInstance(Behavior& behavior);
};
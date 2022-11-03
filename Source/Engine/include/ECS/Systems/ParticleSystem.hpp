#pragma once

#include <vector>
#include <memory>

#include <Refureku/TypeInfo/Archetypes/Struct.h>

#include "EngineDll.hpp"
#include "Tools/Event.hpp"

class ParticleComponent;
class GameObject;
class HYGUID;

/**
@brief System that handle every particle components in the game
*/
class ParticleSystem
{
private:
	std::vector<std::unique_ptr<ParticleComponent>> m_particleComponents;

	/**
	@brief Flag to know if the system has to call Awake/Start functions on behavior registering
	*/
	bool m_shouldInitiateParticle = false;
	bool m_addParticle = false;

	// Event called when a behavior is added in game (Awake/Start)
	Event<> m_awakeInitialize;
	Event<> m_startInitialize;

	/**
	@brief Invoke and clear initialize events
	*/
	void InitializeNewParticles();

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
	@brief Get all particles components taht were registered
	
	@return  const std::vector<ParticleComponent*>& particle component list
	*/
	ENGINE_API const std::vector<std::unique_ptr<ParticleComponent>>& GetAllParticleComponents() const;

	/**
	@brief Call Awake function for all particles components
	*/
	ENGINE_API void AwakeAll();

	/**
	@brief Call Start function for all particles components
	*/
	ENGINE_API void StartAll();

	/**
	@brief Call Stop function for all particles components
	*/
	ENGINE_API void StopAll();

	/**
	@brief Call Update function for all particle components

	@param tick : Engine deltaTime
	*/
	ENGINE_API void UpdateAll(float tick);

	/**
	@brief Call FixedUpdate function for all particle components

	@param fixedTick : Engine fixedDeltaTime
	*/
	ENGINE_API void FixedUpdateAll(float fixedTick);

	/**
	@brief Add a new particle component to the system

	@param owner : Owner of the particle component
	@param id : Unique identifier of the particle component

	@return ParticleComponent* : Created particle component, nullptr if not valid
	*/
	ParticleComponent* AddParticleInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id);

	/**
	@brief Remove an existing particle component in the system

	@param particleComp : particle component to remove
	*/
	void RemoveParticleInstance(ParticleComponent& particleComp);
};
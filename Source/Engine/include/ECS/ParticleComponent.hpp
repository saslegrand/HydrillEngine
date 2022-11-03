#pragma once

#include <unordered_map>

#include "ECS/Component.hpp"
#include "Maths/Vector3.hpp"
#include "Particles/ParticleEmitter.hpp"

#include "Generated/ParticleComponent.rfkh.h"

class GameObject;

/**
@brief Particle component
*/
class HY_CLASS() ParticleComponent : public Component
{

private:

	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<ParticleComponent> defaultInstantiator(GameObject & GO, const HYGUID & uid);

//	Constructors & Destructors

public:

	ParticleComponent(GameObject& gameObject, const HYGUID& uid);
	ParticleComponent(ParticleComponent const& cam) = delete;
	ParticleComponent(ParticleComponent && cam) = default;

	ParticleComponent& operator=(ParticleComponent const& cam) = delete;
	ParticleComponent& operator=(ParticleComponent && cam) = default;

	~ParticleComponent();

//	Variables

private:

	//	Avoid to add particle emitter in game for now
	//	If a particle is added while partile is played then don't call its functions since gameObject reference was not passed to the emitter, that may be dangerous
	//	The OnMofify will call the awake function to reinitialize the emitters, this is a work around for now while waiting for more reflection functionnalities that may be helpful or this case
	unsigned int m_emittercount = 0;

	float m_elapsedTime = 0.0f;
	bool  m_playing = false;
	bool  m_started = false;

public:
	
	HY_FIELD(PHeader("Effect parameters")) bool  loop = true;
	HY_FIELD() bool autoDestroyGameObject = false;
	HY_FIELD() float duration = 1.f;
	HY_FIELD() float gapDuration = 1.f;

	HY_FIELD() HyVector<ParticleEmitter> emitters;

//	Functions

private:

	/**
	@brief First called function after the component was loaded
	*/
	void Initialize();

public:
	
	/**
	@brief Particle component awake function, Initialize the emitters 
	*/
	void Awake();

	/**
	@brief Particle component start function, start all the emitters
	*/
	void Start();

	/**
	@brief Particle component stop function, stop all the emitters, but keep in mind that instanced particles will still be display until their death, 
	use force stop if you want to stop all of them in one go
	*/
	void Stop();

	/**
	@brief Particle component force stop function will stop everything and kill all particles
	*/
	void ForceStop();

	/**
	@brief Particle component update function will update all particles emitters
	*/
	void Update(float tick);

	/**
	@brief Particle component fixed update function will call fixed update functions from all particle emitters

	@param fixedTick : fixed deltaTime
	*/
	void FixedUpdate(float fixedTick);

	/**
	@brief OnModify function is called when a change occured in the editor
	*/
	void OnModify() override;

	/**
	@brief Change Active state of the particle component, keep in mind that if the component is Active, 
	it will automatically be played, and if disabled it will force kill all particles instances

	@param activation value
	*/
	void SetActive(bool value) override;

	ParticleComponent_GENERATED
};

File_ParticleComponent_GENERATED
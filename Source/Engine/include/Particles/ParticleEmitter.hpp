#pragma once

#include <queue>
#include "Maths/Vector3.hpp"
#include "Particles/ParticleInstance.hpp"
#include "Resources/Resource/Texture.hpp"
#include "Core/BaseObject.hpp"

#include "Generated/ParticleEmitter.rfkh.h"

class GameObject;

struct HY_STRUCT() ParticleTexture : public BaseObject
{
	HY_FIELD(POnModify("GenerateTexture")) Texture * texture = nullptr;
	HY_FIELD() float weight     = 1.f;

	HY_FIELD(PHeader("UV parameters")) 
			   Vector2 tiling      = Vector2::One;
	HY_FIELD() Vector2 offset      = Vector2::Zero;
	HY_FIELD() Vector2 offsetSpeed = Vector2::Zero;

	HY_METHOD()
	void GenerateTexture();

	ParticleTexture_GENERATED
};

/**
@brief The particle emitter is a particle instance source, all particles emitted by an emitter will have the same parameters.
*/
class HY_CLASS() ParticleEmitter : public BaseObject
{

public:

	enum class HY_ENUM() EmitterState : short
	{
		Spawning,	//	Particles are spawning
		Playing,	//	No particle is spawning, but particles are currently alive
		Finished,	//	No particle is currently alive
		Inactive,	//	The particle emitter is disabled
	};

	enum class HY_ENUM() SpawnShape
	{
		Sphere,
		Cone,
	};

//	Variables

private:
	
	size_t spawnIndex = 0;

public:
	GameObject* gameObject;

	//	Shaders

	std::string vertexShaderCode;
	std::string fragmentShaderCode;

	// Emitter Datas

	EmitterState state = EmitterState::Inactive;

	bool allowParticleRespawnOnDeath = true;

	float elapsedTimeSpawning		= 0.0f;
	float elapsedTimeSinceLastBurst = 0.0f;

	std::vector<ParticleInstance> instances;

	//	Emitter Parameters

	HY_FIELD(PHeader("Particles behaviour"))
			   bool billboard		= false;
	HY_FIELD() bool useGravity		= true;
	HY_FIELD() bool worldSpace		= true;
	HY_FIELD() bool blending		= false;
	HY_FIELD() bool particleLoop = false;

	HY_FIELD(PHeader("Emission parameters"))
			   int   maxInstance = 1000;
	HY_FIELD() float spawnDuration = 0.05f;

	HY_FIELD(PHeader("Textures"))
	HY_FIELD() ParticleTexture diffuseTexture;
	HY_FIELD() ParticleTexture emissiveTexture;
	HY_FIELD() ParticleTexture blendTexture01;
	HY_FIELD() ParticleTexture blendTexture02;
	HY_FIELD() ParticleTexture blendTexture03;
	HY_FIELD() ParticleTexture blendTexture04;

	HY_FIELD(PHeader("Spawn parameters"))
			   SpawnShape spawnShape = SpawnShape::Sphere;
	HY_FIELD() float spawnRange = 0.5f;
	HY_FIELD() float spawnConeAngle = 75.f;

	HY_FIELD(PHeader("Burst parameters"))
		       SpawnShape burstShape = SpawnShape::Cone;
	HY_FIELD() float burstDirectionConeAngle = 75.f;


	//	Particle Parameters

	HY_FIELD(PHeader("Particles parameters")) 
			   ParticleParameters parameters;

//	Constructors

public:

	ENGINE_API ParticleEmitter();
	ENGINE_API ~ParticleEmitter();

//	Functions

private:

	/**
	@brief Resize the particle instance list
	*/
	void ResizeInstanceList();

	/**
	@brief Spawn particles update function

	@param tick : deltatTime
	*/
	void SpawnParticles(float tick);

public:

	/**
	@brief Generate particle emitter GPU datas
	*/
	void Initialize(GameObject& parentGameObject);

	/**
	@brief Update function of the emitter will call update functions of all instanced particle emitted by it

	@param tick : deltaTime
	*/
	void Update(float tick);

	/**
	@brief FixedUpdate function of the emitter will call FixedUpdate functions of all instanced particle emitted by it

	@param fixedTick : fixed deltaTime
	*/
	void FixedUpdate(float fixedTick);

	/**
	@brief Start an emittion cycle
	*/
	void Start();

	/**
	@brief Stop particle spawning and disable particle looping if enabled
	*/
	void Stop();

	/**
	@brief Kill all particle instances emitted by this emitter
	*/
	void Clear();

	ParticleEmitter_GENERATED
};

File_ParticleEmitter_GENERATED
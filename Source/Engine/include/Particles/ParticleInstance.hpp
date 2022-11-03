#pragma once

#include <Maths/Matrix4.hpp>
#include <ECS/TransformData.hpp>
#include <Particles/ParticleParameters.hpp>

class ParticleEmitter;

class ParticleInstance
{

//	Variables

private:

	//	Instance Datas
	float m_seed = 0.0f;

	float	m_life = 0.0f;
	float	m_age = 0.0f;
	float	m_ageRatio = 0.0f;

	//	Datas

	Vector3	m_velocity;
	Vector3 m_sumForces;

	Matrix4* m_parentMatrix = nullptr;
	TransformData m_transform;

	//	Parameters

	ParticleParameters m_parameters;


public:

	Matrix4 globalTransformModel;
	Color4  color;
	Color3  emissiveColor;
	float   emissiveStrength = 1.0f;
	float   billboardRotation = 0.0f;

	//	Shader datas

	Vector4 diffuseUV  = { 0.0f,0.0f,1.0f,1.0f };
	Vector4 emissiveUV = { 0.0f,0.0f,1.0f,1.0f };
	Vector4 BlendUV_01 = { 0.0f,0.0f,1.0f,1.0f };
	Vector4 BlendUV_02 = { 0.0f,0.0f,1.0f,1.0f };
	Vector4 BlendUV_03 = { 0.0f,0.0f,1.0f,1.0f };
	Vector4 BlendUV_04 = { 0.0f,0.0f,1.0f,1.0f };

//	Constructors

public:

	ENGINE_API ParticleInstance(const ParticleEmitter& emitter);
	ENGINE_API ~ParticleInstance();

//	Functions

private:

	//	Compute functions

	/**
	@brief Update instance transform matrices
	*/
	void UpdateTransformMatrix();

public:

	/**
	@brief Update instance

	@param tick : deltaTime
	*/
	void Update(const ParticleEmitter& emitter, float tick);

	/**
	@brief Update instance

	@param fixedTick : fixed deltaTime
	*/
	void FixedUpdate(const ParticleEmitter& emitter, float fixedTick);

	/**
	@brief Spawn instance, and get all parameters from the emitter
	*/
	void Spawn(const ParticleEmitter& emitter);

	/**
	@brief Is the particle alive

	@return true if alive
	*/
	bool IsAlive() const;

	/**
	@brief Get instance world position

	@return Vector3 of the instance position
	*/
	Vector3	GetPosition() const;

	/**
	@brief Get instance world rotation

	@return Quaternion of the instance rotation
	*/
	Quaternion GetRotation() const;

	/**
	@brief Get instance world scale

	@return Vector3 of the instance scale
	*/
	Vector3 GetScale() const;
};
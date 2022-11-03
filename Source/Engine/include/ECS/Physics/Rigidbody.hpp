#pragma once

#include "ECS/Component.hpp"
#include "Maths/Vector3.hpp"
#include "Physics/PhysicsEnums.hpp"
#include "Physics/BoolVector3.hpp"

#include "Generated/Rigidbody.rfkh.h"

namespace physx { class PxRigidDynamic;}
using namespace physx;

class GameObject;

/**
@brief A Rigidbody (with at least one collider) enables the simulation of physics on the GameObject that it's attached to
*/
class HY_CLASS() Rigidbody : public Component
{
	friend class PhysicsActor;

private:
	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<Rigidbody> defaultInstantiator(GameObject & GO, const HYGUID & uid);

private:
	PxRigidDynamic* m_RigidDynamic = nullptr;

	HY_FIELD() float m_mass = 1.0f;
	HY_FIELD() float m_density = 1.0f;
	HY_FIELD() float m_drag = 0.2f;
	HY_FIELD() float m_angularDrag = 0.02f;
	HY_FIELD() bool m_useGravity = true;
	HY_FIELD() bool m_isKinematic = false;
	HY_FIELD() InterpolationMode m_interpolate = InterpolationMode::None;
	HY_FIELD() CollisionDetectionMode m_collisionDetection = CollisionDetectionMode::Discrete;
	HY_FIELD() BoolVector3 m_freezePosition = { 0, 0, 0 };
	HY_FIELD() BoolVector3 m_freezeRotation = { 0, 0, 0 };

public:
	ENGINE_API Rigidbody(GameObject& gameObject, const HYGUID& uid);
	Rigidbody(Rigidbody const& cam) = delete;
	Rigidbody(Rigidbody && cam) = default;

	Rigidbody& operator=(Rigidbody const& cam) = delete;
	Rigidbody& operator=(Rigidbody && cam) = default;

	virtual ~Rigidbody();

private:
	void PrepareForSimulation();

public:

	/**
	@brief Get the PxActor reference the rigidbody is linked to
	@return PxActor* : PhysX actor reference
	*/
	ENGINE_API PxActor* GetPxActor();

	// Get & Set parameter : Mass
	// --------------------------

	/**
	@brief Get the mass of the rigidbody
	@return float : mass
	*/
	ENGINE_API float GetMass() const;

	/**
	@brief Set the mass of the rigidbody
	@param mass : float
	*/
	ENGINE_API void SetMass(float mass);

	// Get & Set parameter : Density
	// --------------------------

	/**
	@brief Get the density of the rigidbody
	@return float : density
	*/
	ENGINE_API float GetDensity() const;

	/**
	@brief Set the density of the rigidbody
	@param density : float
	*/
	ENGINE_API void SetDensity(float density);


	// Get & Set parameter : Drag
	// --------------------------

	/**
	@brief Get the drag of the rigidbody
	@return float : drag
	*/
	ENGINE_API float GetDrag() const;

	/**
	@brief Set the mass of the rigidbody
	@param mass : float
	*/
	ENGINE_API void SetDrag(float drag);


	// Get & Set parameter : Angular Drag
	// ----------------------------------

	/**
	@brief Get the angular drag of the rigidbody
	@return float : angular drag
	*/
	ENGINE_API float GetAngularDrag() const;

	/**
	@brief Set the angular drag of the rigidbody
	@param angularDrag : float
	*/
	ENGINE_API void SetAngularDrag(float angularDrag);


	// Get & Set parameter : Use Gravity
	// ---------------------------------

	/**
	@brief Get if the rigidbody is using gravity
	@return bool : true if using gravity, false if not
	*/
	ENGINE_API bool IsUsingGravity() const;

	/**
	@brief Set if the rigidbody is using gravity
	@param value : bool
	*/
	ENGINE_API void SetUseGravity(bool value);


	// Get & Set parameter : Is Kinematic
	// ----------------------------------

	/**
	@brief Get if the rigidbody has been set to kinematic
	@return bool : true if is kinematic, false if not
	*/
	ENGINE_API bool IsKinematic() const;

	/**
	@brief Set if the rigidbody has been set to kinematic
	@param value : bool
	*/
	ENGINE_API void SetIsKinematic(bool value);


	// Get & Set parameter : Interpolation Mode
	// ----------------------------------------

	/**
	@brief Get the interpolation mode of the rigidbody
	@return InterpolationMode : interpolation mode enum
	*/
	//InterpolationMode GetInterpolation();
	
	/**
	@brief Set the interpolation mode of the rigidbody
	@param mode : InterpolationMode
	*/
	//void SetInterpolation(InterpolationMode mode);


	// Get & Set parameter : Collision Detection Mode
	// ----------------------------------------------

	/**
	@brief Get the collision detection mode of the rigidbody
	@return CollisionDetectionMode : collision detection mode enum
	*/
	CollisionDetectionMode GetCollisionDetectionMode() const;

	/**
	@brief Set the collision detection mode of the rigidbody
	@param mode : CollisionDetectionMode
	*/
	void SetCollisionDetectionMode(CollisionDetectionMode mode);


	// Get & Set parameter : Constraints
	// ---------------------------------

	/**
	@brief Get the constraints of the rigidbody
	@return const Constraints& : constraints
	*/
	const BoolVector3& GetPositionConstraints() const;

	/**
	@brief Set the constraints of the rigidbody
	@param constraints : Constraints
	*/
	void SetPositionConstraints(const BoolVector3& constraints);

	/**
	@brief Get the constraints of the rigidbody
	@return const Constraints& : constraints
	*/
	const BoolVector3& GetRotationConstraints() const;

	/**
	@brief Set the constraints of the rigidbody
	@param constraints : Constraints
	*/
	void SetRotationConstraints(const BoolVector3& constraints);



	/**
	@brief Applies a force(or impulse) defined in the global coordinate frame, to the actor, at its center of mass.
	*/
	ENGINE_API void AddForce(const Vector3& force, const ForceMode& forceMode = ForceMode::eFORCE);

	/**
	@brief Applies an impulsive torque defined in the global coordinate frame, to the actor.
	*/
	ENGINE_API void AddTorque(const Vector3& torque, const ForceMode& forceMode = ForceMode::eFORCE);

	/**
	@brief Get the current linear velocity vector of the Rigidbody
	@return Vector3 : Linear Velocity
	*/
	ENGINE_API Vector3 GetVelocity();

	/**
	@brief Get the current angular velocity vector of the Rigidbody
	@return Vector3 : Angular Velocity (axis of rotation, magnitude is angle)
	*/
	ENGINE_API Vector3 GetAngularVelocity();

	Rigidbody_GENERATED
};

File_Rigidbody_GENERATED

#pragma once

#include <vector>
#include <memory>

#include <Refureku/TypeInfo/Archetypes/Struct.h>

#include "EngineDLL.hpp"

namespace physx
{
	class PxRigidActor;
}
using namespace physx;

class PhysicsSimulation;

class GameObject;
class HYGUID;
class Collider;
class TerrainCollider;
class Rigidbody;

/**
@brief Actor used in the Physics Simulation. Make the link between one GameObject (with Colliders/Rigidbody components) and one PxRigidActor (PhysX);
*/
class PhysicsActor
{
private:
	PhysicsSimulation& m_simulation;
	// Reference to the simulation for PxObject creation

	GameObject& m_gameObject;
	// GameObject reference : Only one PhysicsActor by GameObjects that need one

	Rigidbody* m_rigidbody = nullptr;
	std::vector<Collider*> m_colliders;
	// Linked GameObject's components

	bool m_isDynamic = false;
	PxRigidActor* m_RigidActor = nullptr;
	// Static or dynamic actor

public:
	PhysicsActor(GameObject& gameObject, PhysicsSimulation& simulation);

	/**
	@brief Add rigidbody reference to this physics actor (can only have one)
	@param rigidbody : Rigidbody*
	*/
	void AddRigidbody(Rigidbody& rigidbody);

	/**
	@brief Remove rigidbody reference (can only have one)
	*/
	void RemoveRigidbody();


	/**
	@brief Add collider reference to this physics actor
	@param collider : Collider*
	*/
	void AddCollider(Collider& collider);

	/**
	@brief Remove collider reference
	@param collider : Collider*
	*/
	void RemoveCollider(Collider& collider);

	/**
	@brief Remove the PxShape that could have been created on the actor
	*/
	void RemoveShapes();

	/**
	@brief Remove the actor from the simulation
	*/
	void RemoveFromSimulation();

	/**
	@brief Create the PxRigidActor*, all PxShape*, and link them to the Physics Simulation
	*/
	void PrepareForSimulation();


	/**
	@brief Update the GameObject's transform according to PxActor transform (that has been updated by PhysX)
	*/
	void RetreiveTransform();

	/**
	@brief Update the PxActor transform according to GameObject's transform (that may have been updated by scripting)
	*/
	void SendTransform();


	/**
	@brief Get PxRigidActor* of this actor. Should be nullptr if physics simulation is off.
	@return PxRigidActor* : PxRigidStatic* or PxRigidDynamic* of this actor
	*/
	PxRigidActor* GetRigidActor();

	/**
	@brief Get the GameObject reference associated with this actor
	*/
	ENGINE_API GameObject& GetGameObject();

	/**
	@brief Check if the actor is empty from any colliders/rigidbody. If true it should be deleted soon because it's useless now.
	@return bool : true if empty, false if not
	*/
	bool IsEmpty();


	void DrawDebug();
};



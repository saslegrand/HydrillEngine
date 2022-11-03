#pragma once

#include <vector>
#include <memory>
#include <Refureku/TypeInfo/Archetypes/Struct.h>

#include "EngineDLL.hpp"

#include "Physics/PhysicsSimulation.hpp"
#include "Physics/PhysicsActor.hpp"

class GameObject;
class Collider;
class Rigidbody;

/**
@brief Component System responsible of handling all PhysicsActor components and the PhysicsSimulation
*/
class PhysicsSystem
{
private:
	PhysicsSimulation m_simulation;
	bool running = false;

	std::vector<std::unique_ptr<Rigidbody>> m_rigidbodies;
	std::vector<std::unique_ptr<Collider>> m_colliders;
	std::vector<std::unique_ptr<PhysicsActor>> m_actors;
	// TODO : maybe replace vector by unordered_map<GameObject*, PhysicsActor> ?
	// It could improve finding and removing

public:
	PhysicsSystem();
	~PhysicsSystem();

private:

	/**
	@brief Try to update actor if game is running
	@param actor : PhysicsActor&
	*/
	void UpdateActor(PhysicsActor& actor);

	/**
	@brief Remove specific actor from actors list
	@param actor : PhysicsActor&
	*/
	void RemoveActor(PhysicsActor& actor);

	/**
	@brief Find actor inside actors list with a given GameObject reference
	@param gameObject : GameObject& potentially associated to an actor
	*/
	PhysicsActor* FindActor(GameObject& gameObject);

public:

	/**
	@brief Add a new collider to the system

	@param archetype : Archetype of the collider
	@param owner : Owner of the collider
	@param id : Unique identifier of the collider

	@return Collider* : Created collider, nullptr if not valid
	*/
	Collider* AddColliderInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id);

	/**
	@brief Remove an existing collider in the system

	@param collider : collider to remove
	*/
	void RemoveColliderInstance(Collider& collider);

	/**
	@brief Add a new rigidbody to the system

	@param archetype : Archetype of the rigidbody
	@param owner : Owner of the rigidbody
	@param id : Unique identifier of the rigidbody

	@return Collider* : Created rigidbody, nullptr if not valid
	*/
	Rigidbody* AddRigidbodyInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id);

	/**
	@brief Remove an existing rigidbody in the system

	@param rigidbody : rigidbody to remove
	*/
	void RemoveRigidbodyInstance(Rigidbody& rigidbody);

	/**
	@brief Update at a fixed timestep the physics simulation
	@param fixedDeltaTime : float - fixed tick delta time
	*/
	void FixedUpdate(float fixedDeltaTime);
	void SendTransforms();
	void ReceiveTransforms();

	/**
	@brief Set-up to start simulating physics in the current scene.
	For that it retreive all the PhysicsActors and it put them inside the simulation.
	*/
	ENGINE_API void StartPhysics();

	/**
	@brief Stop the simulation and clean all PhysicsActors
	*/
	ENGINE_API void StopPhysics();

	/**
	@brief Create the simulation scene, so that physics actors can be placed in the scene
	*/
	ENGINE_API void CreateSimulation();

	/**
	@brief Destroy the simulation scene, removing all physics actors
	*/
	ENGINE_API void DestroySimulation();

	/**
	@brief Get the physx simulation. For internal use only
	*/
	PhysicsSimulation& GetSimulation();

	/**
	@brief Launch a raycast from origin to direction, and get back the first hit that blocks, with ignoring self actor
	@param origin : Vector3& - ray world position origin
	@param unitDirection : Vector3& - unit vector, the direction of the ray
	@param maxDistance : float - max ray distance
	@param outHit : PxRaycastHit& - reference output of the hit, valid if the function return true
	@param selfActor : PxActor* - reference of the rigidbody to ignore self
	@param layerMask : PxU32 - layer binary mask to avoid checking collisions with certain layers
	*/
	ENGINE_API static bool RaycastIgnoreSelf(const Vector3& origin, const Vector3& unitDirection, float maxDistance, PxRaycastHit& outHit, PxActor* selfActor, PxU32 layerMask = UINT32_MAX);

	/**
	@brief Launch a raycast from origin to direction, and get back the list of all hits that collides with the ray
	@param origin : Vector3& - ray world position origin
	@param unitDirection : Vector3& - unit vector, the direction of the ray
	@param maxDistance : float - max ray distance
	@param outHit : PxRaycastBuffer& - reference output of the hits, valid if the function return true
	@param layerMask : PxU32 - layer binary mask to avoid checking collisions with certain layers
	*/
	ENGINE_API static bool Raycast(const Vector3& origin, const Vector3& unitDirection, float maxDistance, PxRaycastBuffer& outHit, PxU32 layerMask = UINT32_MAX);

	/**
	@brief Internal raycast launch, used by raycast
	*/
	bool RaycastLaunch(const Vector3& origin, const Vector3& unitDirection, float maxDistance, PxRaycastBuffer& outHit, PxU32 layerMask);

	/**
	@brief Draw debug of all physics actors
	*/
	ENGINE_API void DrawDebug();
};

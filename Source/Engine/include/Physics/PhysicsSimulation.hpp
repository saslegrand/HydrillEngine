#pragma once

#include <ctype.h>

#define PX_PHYSX_STATIC_LIB
#include <physx/PxPhysicsAPI.h>
//namespace physx
//{
//	class PxDefaultAllocator;
//	class PxDefaultErrorCallback;
//	class PxFoundation;
//	class PxPhysics;
//	class PxDefaultCpuDispatcher;
//	class PxScene;
//	class PxMaterial;
//	class PxPvd;
//	class PxShape;
//	class PxActor;
//	class PxRigidDynamic;
//	class PxRigidStatic;
//}
using namespace physx;

class Collider;
class Transform;
class Terrain;



#define PVD_HOST "127.0.0.1"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.
#define PX_RELEASE(x) if(x) {x->release(); x = nullptr;}

#include "Physics/PhysicsEnums.hpp"
#include "Physics/PhysicsMaterial.hpp"

#include "Maths/Vector3.hpp"


/**
@brief Encapsulate PhysX simulation scene and give access to PhysX object creation functions
*/
class PhysicsSimulation : public PxSimulationEventCallback
{
private:
	PxDefaultAllocator m_allocator;
	PxDefaultErrorCallback m_errorCallback;

	PxFoundation* m_foundation = nullptr;
	PxPhysics* m_physics = nullptr;
	PxCooking* m_cooking = nullptr;

	PxDefaultCpuDispatcher* m_dispatcher = nullptr;
	PxScene* m_scene = nullptr;

	PxMaterial* m_material = nullptr;

	// Debug mode with PhysX Visual Debugger
	bool m_visualDebug = true;
	PxPvd* m_pvd = nullptr;

	//bool LayerCollisionMatrix[36] =
	//{
	//	1, 1, 1, 1, 1, 1, 1, 1,
	//	1, 1, 1, 1, 1, 1, 1,
	//	1, 1, 1, 1, 1, 1,
	//	1, 1, 1, 1, 1,
	//	1, 1, 1, 1,
	//	1, 1, 1,
	//	1, 1,
	//	1
	//};

public:

	/**
	@brief Create the PhysX simulation, set-up the scene, and should be ready to be run every frame with StepSimulation()
	*/
	void Create();

	/**
	@brief Destroy the PhysX simulation and all the objects associated with it.
	*/
	void Destroy();

	/**
	@brief Compute a step in the PhysX simulation
	@param timeStep : duration of the step (should be set between 1/50 & 1/60 for a good performance/quality compromise)
	*/
	void StepSimulation(float timeStep);


	/**
	@brief Create a PxShape alter-ego of the given collider, with a given PxMaterial
	@param collider : Collider to create shape from
	@param material : PxMaterial reference for the shape
	@return PxShape* : reference to the PxShape created
	*/
	PxShape* CreateShape(Collider* collider, PxMaterial* material);

	/**
	@brief Create a PxMaterial alter-ego of the give material
	@param material : PhysicsMaterial to create PxMaterial from
	@return PxMaterial* : reference to the PxMaterial created
	*/
	PxMaterial* CreateMaterial(const PhysicsMaterial& material);

	/**
	@brief Create a PxRigidDynamic actor, used with a Rigidbody
	@param transform : actor transform
	@return PxRigidDynamic* : reference to the PxRigidDynamic created
	*/
	PxRigidDynamic* CreateDynamicActor(const Transform& transform);

	/**
	@brief Create a PxRigidStatic actor, used with colliders-only objects
	@param transform : actor transform
	@return PxRigidStatic* : reference to the PxRigidStatic created
	*/
	PxRigidStatic* CreateStaticActor(const Transform& transform);

	/**
	@brief Generate a terrain and its mesh from its attached texture
	@param terrain : terrain reference
	@return bool : return true if success, false otherwise
	*/
	static bool GenerateTerrain(Terrain* terrain);

	/**
	@brief Create a PxHeightField from a description data
	@param heightFieldDesc : height field description
	@return PxHeightField* : reference to the PxHeightField created
	*/
	PxHeightField* CreateHeightField(const PxHeightFieldDesc& heightFieldDesc);

	/**
	@brief Add a given PxActor to the current PhysX simulation scene
	@param actor : PxActor& to add
	*/
	void AddActorToSimulation(PxActor& actor);

	/**
	@brief Remove a given PxActor from the current PhysX simulation scene
	@param actor : PxActor& to remove
	*/
	void RemoveActorFromSimulation(PxActor& actor);

	/**
	@brief Launch a ray in the simulation and retreive hits
	*/
	bool Raycast(const Vector3& origin, const Vector3& unitDirection, float maxDistance, PxRaycastBuffer& outHit, PxU32 layerMask);

	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {};
	virtual void onWake(PxActor** actors, PxU32 count) override {};
	virtual void onSleep(PxActor** actors, PxU32 count) override {};
	virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override {};
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
};

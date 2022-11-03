#pragma once

#include "ECS/Component.hpp"

#include "Physics/PhysicsMaterial.hpp"
#include "Physics/PhysicsEnums.hpp"

#include "Maths/Vector3.hpp"

#include "Tools/Event.hpp"

#include "Generated/Collider.rfkh.h"

namespace physx
{
class PxShape;
class PxMaterial;
class PxGeometry;
}
using namespace physx;

class GameObject;

struct CollisionInfo
{
	class Collider* other;
	Vector3 point;
	Vector3 normal;
};

// Offset when drawing debug collider shapes, to avoid Z-Fighting
#define DRAW_DEBUG_OFFSET 0.01f

/**
@brief Base class for Colliders Components. Can't be instantiated.
*/
class HY_CLASS() ENGINE_API Collider : public Component
{
	friend class PhysicsActor;

private:
	PxActor* m_pxActor = nullptr;

protected:
	/** @brief Enum type to distinguish different collider shapes */
	ColliderShapeType m_shapeType = ColliderShapeType::eINVALID;
	// Should be set in the constructor of the classes that inherit from collider


	PxShape*		  m_PxShape = nullptr;
	PxMaterial*		  m_PxMaterial = nullptr;
	// Collider's PhysX alter-egos


	HY_FIELD() bool			  m_isTrigger = false;
	HY_FIELD() PhysicsMaterial   m_material; // TODO : Maybe turn this into a resource, so that multiple colliders can point to the same material
	HY_FIELD() Vector3			  m_center = Vector3::Zero;
	// Common parameters between all colliders

protected:
	Collider(GameObject& gameObject, const std::string& name, const HYGUID& uid);
	Collider(Collider const& cam) = delete;
	Collider(Collider&& cam) = default;

	Collider& operator=(Collider const& cam) = delete;
	Collider& operator=(Collider && cam) = default;

public:
	virtual ~Collider();

public:

	/**
	@brief Get the PxActor reference the rigidbody is linked to
	@return PxActor* : PhysX actor reference
	*/
	PxActor* GetPxActor();

	/**
	@brief Set-up the PxGeometry according to parameters before PxShape creation
	*/
	virtual void PrepareGeometry() = 0;

	/**
	@brief Set-up the PxMaterial and the PxShape according to parameters
	*/
	virtual void PrepareForSimulation();

	/**
	@brief Get the geometry shape type
	@return ColliderShapeType : PxGeometry enum shape type
	*/
	ColliderShapeType GetShapeType();

	/**
	@brief Get the PxGeometry of this collider for the PxShape creation
	@return const PxGeometry& : PxGeometry member
	*/
	virtual const PxGeometry& GetGeometryShape() const = 0;


	/**
	@brief Get if this collider is a trigger-only collider or not
	@return bool : true if it's a trigger, false if not
	*/
	bool IsTrigger();

	/**
	@brief Set if this collider is a trigger-only collider or not
	@param value : bool
	*/
	void SetIsTrigger(bool value);


	/**
	@brief Get the PhysicsMaterial associated with this collider
	@return const PhysicsMaterial& : PhysicsMaterial member
	*/
	const PhysicsMaterial& GetMaterial();

	/**
	@brief Set the PhysicsMaterial associated with this collider
	@param material : const PhysicsMaterial&
	*/
	void SetMaterial(const PhysicsMaterial& material);


	/**
	@brief Get the local position of the center of this collider
	@return const Vector3& : center
	*/
	const Vector3& GetCenter();

	/**
	@brief Set the local position of the center of this collider
	@param center : const Vector3&
	*/
	void SetCenter(const Vector3& center );

	virtual void DrawDebug() {};

#pragma warning(disable:4251)
	Event<CollisionInfo> OnCollisionEnter;
	Event<CollisionInfo> OnCollisionStay;
	Event<CollisionInfo> OnCollisionExit;
	Event<CollisionInfo> OnTriggerEnter;
	Event<CollisionInfo> OnTriggerExit;
#pragma warning(default:4251)
	

	Collider_GENERATED
};

File_Collider_GENERATED

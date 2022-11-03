
#include "EngineContext.hpp"
#include "Physics/PhysicsSystem.hpp"
#include "ECS/GameObject.hpp"

#include "Generated/PhysicsMaterial.rfks.h"
#include "Generated/Collider.rfks.h"

Collider::Collider(GameObject& gameObject, const std::string& name, const HYGUID& uid)
	: Component(gameObject, name, uid)
{
	
}

Collider::~Collider()
{
	//EngineContext::Instance().physicsSystem->Unregister(*this);
}

PxActor* Collider::GetPxActor()
{
	return m_pxActor;
}

ColliderShapeType Collider::GetShapeType()
{
	return m_shapeType;
}

bool Collider::IsTrigger()
{
	return m_isTrigger;
}

void Collider::SetIsTrigger(bool value)
{
	m_isTrigger = value;

	if (m_PxShape != nullptr)
	{
		if (value)
		{
			m_PxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false); // Not a collider
			m_PxShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false); // Disable raycast to hit the trigger
			m_PxShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true); // This is a trigger
		}
		else
		{
			m_PxShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false); // Not a trigger
			m_PxShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true); // Enable raycast to hit the collider
			m_PxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true); // This is a collider
		}
		
	}
}

const PhysicsMaterial& Collider::GetMaterial()
{
	return m_material;
}

void Collider::SetMaterial(const PhysicsMaterial& material)
{
	m_material = material;

	if (m_PxMaterial != nullptr)
	{
		m_PxMaterial->setStaticFriction(material.staticFriction);
		m_PxMaterial->setDynamicFriction(material.dynamicFriction);
		m_PxMaterial->setRestitution(material.restitution);
		m_PxMaterial->setRestitutionCombineMode(material.combineMode);
	}
}

const Vector3& Collider::GetCenter()
{
	return m_center;
}

void Collider::SetCenter(const Vector3& center)
{
	m_center = center;

	if (m_PxShape != nullptr)
	{
		Vector3 pos = gameObject.transform.Scale() * m_center;
		PxTransform transform = PxTransform::PxTransform(PxVec3(pos.x, pos.y, pos.z));
		m_PxShape->setLocalPose(transform);
	}
}

void Collider::PrepareForSimulation()
{
	assert(m_PxShape && "Collider can't prepare for simulation because of PxShape was not created. This should not happen");

	m_PxShape->userData = this;

	SetIsTrigger(m_isTrigger);
	SetMaterial(m_material);
	SetCenter(m_center);
}
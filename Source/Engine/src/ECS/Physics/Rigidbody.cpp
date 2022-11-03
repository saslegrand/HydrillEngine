
#include <physx/PxRigidDynamic.h>

#include "EngineContext.hpp"
#include "Physics/PhysicsSystem.hpp"
#include "ECS/GameObject.hpp"

#include "Core/Logger.hpp"

#include "Generated/BoolVector3.rfks.h"
#include "Generated/PhysicsEnums.rfks.h"
#include "Generated/Rigidbody.rfks.h"

rfk::UniquePtr<Rigidbody> Rigidbody::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<Rigidbody>(GO, uid);
}

Rigidbody::Rigidbody(GameObject& gameObject, const HYGUID& uid)
	: Component(gameObject, staticGetArchetype().getName(), uid)
{
	m_isUnique = true;
	//EngineContext::Instance().physicsSystem->Register(*this);
}

Rigidbody::~Rigidbody()
{
	//EngineContext::Instance().physicsSystem->Unregister(*this);
}

PxActor* Rigidbody::GetPxActor()
{
	return m_RigidDynamic;
}

float Rigidbody::GetMass() const
{
	return m_mass;
}

void Rigidbody::SetMass(float mass)
{
	if (mass < 0)
	{
		Logger::Warning("Rigidbody::SetMass(mass) : Mass value has not been set to " + std::to_string(mass) + " because it can't be negative");
		return;
	}

	m_mass = mass;
	if (m_RigidDynamic != nullptr)
		m_RigidDynamic->setMass(m_mass);
}

float Rigidbody::GetDensity() const
{
	return m_density;
}

void Rigidbody::SetDensity(float density)
{
	if (density < 0)
	{
		Logger::Warning("Rigidbody::SetDensity(density) : density value has not been set to " + std::to_string(density) + " because it can't be negative");
		return;
	}

	m_density = density;
}

float Rigidbody::GetDrag() const
{
	return m_drag;
}

void Rigidbody::SetDrag(float drag)
{
	if (drag < 0)
	{
		Logger::Warning("Rigidbody::SetDrag(drag) : Drag value has not been set to " + std::to_string(drag) + " because it can't be negative");
		return;
	}

	m_drag = drag;
	if (m_RigidDynamic != nullptr)
		m_RigidDynamic->setLinearDamping(m_drag);
}

float Rigidbody::GetAngularDrag() const
{
	return m_angularDrag;
}

void Rigidbody::SetAngularDrag(float angularDrag)
{
	if (angularDrag < 0)
	{
		Logger::Warning("Rigidbody::SetAngularDrag(angularDrag) : Angular Drag value has not been set to " + std::to_string(angularDrag) + " because it can't be negative");
		return;
	}

	m_angularDrag = angularDrag;
	if (m_RigidDynamic != nullptr)
		m_RigidDynamic->setAngularDamping(angularDrag);
}

bool Rigidbody::IsUsingGravity() const
{
	return m_useGravity;
}

void Rigidbody::SetUseGravity(bool value)
{
	m_useGravity = value;
	if (m_RigidDynamic != nullptr)
		m_RigidDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !value);
}

bool Rigidbody::IsKinematic() const
{
	return m_isKinematic;
}

void Rigidbody::SetIsKinematic(bool value)
{
	m_isKinematic = value;
	if (m_RigidDynamic != nullptr)
		m_RigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, m_isKinematic);
}

//InterpolationMode Rigidbody::GetInterpolation()
//{
//	return m_interpolate;
//}
//
//void Rigidbody::SetInterpolation(InterpolationMode mode)
//{
//	m_interpolate = mode;
//	if (m_RigidDynamic != nullptr)
//	{
//		switch (mode)
//		{
//		case InterpolationMode::None:
//			m_RigidDynamic->
//				break;
//		case InterpolationMode::Interpolate:
//
//			break;
//		case InterpolationMode::Extrapolate:
//
//			break;
//		default:
//			break;
//		}
//	}
//}

CollisionDetectionMode Rigidbody::GetCollisionDetectionMode() const
{
	return m_collisionDetection;
}

void Rigidbody::SetCollisionDetectionMode(CollisionDetectionMode mode)
{
	m_collisionDetection = mode;
	if (m_RigidDynamic != nullptr)
	{
		switch (mode)
		{
		case CollisionDetectionMode::Discrete:
			m_RigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, false);
			break;
		case CollisionDetectionMode::Continuous:
			m_RigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, false);
			m_RigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
			break;
		case CollisionDetectionMode::ContinuousSpeculative:
			m_RigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, false);
			m_RigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, true);
			break;
		default:
			break;
		}
	}
}

const BoolVector3& Rigidbody::GetPositionConstraints() const
{
	return m_freezePosition;
}

void Rigidbody::SetPositionConstraints(const BoolVector3& constraints)
{
	m_freezePosition = constraints;
	if (m_RigidDynamic != nullptr)
	{
		m_RigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, m_freezePosition.X);
		m_RigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, m_freezePosition.Y);
		m_RigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, m_freezePosition.Z);
	}
}

const BoolVector3& Rigidbody::GetRotationConstraints() const
{
	return m_freezeRotation;
}

void Rigidbody::SetRotationConstraints(const BoolVector3& constraints)
{
	m_freezeRotation = constraints;
	if (m_RigidDynamic != nullptr)
	{
		m_RigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, m_freezeRotation.X);
		m_RigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, m_freezeRotation.Y);
		m_RigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, m_freezeRotation.Z);
	}
}


void Rigidbody::PrepareForSimulation()
{
	SetMass(m_mass);
	SetDensity(m_density);
	SetDrag(m_drag);
	SetAngularDrag(m_angularDrag);
	SetUseGravity(m_useGravity);
	SetIsKinematic(m_isKinematic);
	//SetInterpolation(m_interpolate);
	SetCollisionDetectionMode(m_collisionDetection);
	SetPositionConstraints(m_freezePosition);
	SetRotationConstraints(m_freezeRotation);
}



void Rigidbody::AddForce(const Vector3& force, const ForceMode& forceMode)
{
	if (m_RigidDynamic != nullptr)
	{
		m_RigidDynamic->addForce(PxVec3(force.x, force.y, force.z), forceMode);
	}
}

void Rigidbody::AddTorque(const Vector3& torque, const ForceMode& forceMode)
{
	if (m_RigidDynamic != nullptr)
	{
		m_RigidDynamic->addTorque(PxVec3(torque.x, torque.y, torque.z), forceMode);
	}
}

Vector3 Rigidbody::GetVelocity()
{
	PxVec3 velocity = m_RigidDynamic->getLinearVelocity();
	return { velocity.x, velocity.y, velocity.z };
}

Vector3 Rigidbody::GetAngularVelocity()
{
	PxVec3 velocity = m_RigidDynamic->getAngularVelocity();
	return { velocity.x, velocity.y, velocity.z };
}
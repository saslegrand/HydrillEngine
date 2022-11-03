#include "Physics/PhysicsActor.hpp"

#include <physx/PxRigidDynamic.h>
#include <physx/PxRigidStatic.h>

#include "Physics/PhysicsSimulation.hpp"

#include "ECS/GameObject.hpp"
#include "ECS/Physics/Collider.hpp"
#include "ECS/Physics/TerrainCollider.hpp"
#include "ECS/Physics/Rigidbody.hpp"



using namespace physx;

PhysicsActor::PhysicsActor(GameObject& gameObject, PhysicsSimulation& simulation)
	: m_gameObject(gameObject), m_simulation(simulation)
{

}

void PhysicsActor::AddRigidbody(Rigidbody& rigidbody)
{
	m_rigidbody = &rigidbody;
}

void PhysicsActor::RemoveRigidbody()
{
	m_rigidbody = nullptr;
}

void PhysicsActor::AddCollider(Collider& collider)
{
	//collider->m_PxMaterial = m_simulation.CreateMaterial(collider->GetMaterial());
	//collider->m_PxShape = m_simulation.CreateShape(collider, collider->m_PxMaterial);
	m_colliders.emplace_back(&collider);
}

void PhysicsActor::RemoveCollider(Collider& collider)
{
	for (int i = 0; i < m_colliders.size(); ++i)
	{
		if (m_colliders[i] == &collider)
		{
			m_colliders.erase(m_colliders.begin() + i);
			return;
		}
	}
}

void PhysicsActor::RemoveShapes()
{
	for (auto& collider : m_colliders)
	{
		if (collider->m_PxShape)
		{
			collider->m_PxShape->userData = nullptr;
			m_RigidActor->detachShape(*collider->m_PxShape);
			collider->m_PxShape = nullptr;
		}
	}
}


void PhysicsActor::RemoveFromSimulation()
{
	PX_RELEASE(m_RigidActor);
}

void PhysicsActor::PrepareForSimulation()
{
	if (m_gameObject.isStatic)
	{
		if (m_RigidActor == nullptr)
		{
			m_RigidActor = m_simulation.CreateStaticActor(m_gameObject.transform);
			SendTransform();
			m_RigidActor->userData = this;
		}
	}
	else
	{
		if (m_RigidActor == nullptr)
		{
			if (m_rigidbody)
			{
				m_rigidbody->m_RigidDynamic = m_simulation.CreateDynamicActor(m_gameObject.transform);
				m_RigidActor = m_rigidbody->m_RigidDynamic;
				m_RigidActor->userData = this;
				m_rigidbody->PrepareForSimulation();
			}
			else // If no rigidbody, it's a kinematic collider (it can't move)
			{
				m_RigidActor = m_simulation.CreateDynamicActor(m_gameObject.transform);
				m_RigidActor->userData = this;
				if (PxRigidDynamic* rigidDynamic = static_cast<PxRigidDynamic*>(m_RigidActor))
				{
					rigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
				}
			}
		}
	}

	for (Collider* collider : m_colliders)
	{
		collider->PrepareGeometry();
		collider->m_pxActor = m_RigidActor;
		collider->m_PxMaterial = m_simulation.CreateMaterial(collider->GetMaterial());
		collider->m_PxShape = m_simulation.CreateShape(collider, collider->m_PxMaterial);
		collider->PrepareForSimulation();
		m_RigidActor->attachShape(*collider->m_PxShape);
		collider->m_PxShape->release();
		collider->m_PxMaterial->release();
	}

	if (m_rigidbody)
	{
		PxRigidBodyExt::updateMassAndInertia(*(m_rigidbody->m_RigidDynamic), m_rigidbody->GetDensity());
	}
}

void PhysicsActor::RetreiveTransform()
{
	// TODO : We need something in transform to handle world<->local operations
	PxTransform tr = m_RigidActor->getGlobalPose();
	m_gameObject.transform.SetPosition(Vector3(tr.p.x, tr.p.y, tr.p.z));
	m_gameObject.transform.SetRotation(Quaternion(tr.q.x, tr.q.y, tr.q.z, tr.q.w));
}

void PhysicsActor::SendTransform()
{
	Vector3 pos = m_gameObject.transform.Position();
	Quaternion rot = m_gameObject.transform.Rotation();
	PxTransform tr = PxTransform({ pos.x, pos.y, pos.z }, { rot.x, rot.y, rot.z, rot.w });
	m_RigidActor->setGlobalPose(tr);
}



PxRigidActor* PhysicsActor::GetRigidActor()
{
	return m_RigidActor;
}

GameObject& PhysicsActor::GetGameObject()
{
	return m_gameObject;
}

bool PhysicsActor::IsEmpty()
{
	return m_colliders.empty() && m_rigidbody == nullptr;
}

void PhysicsActor::DrawDebug()
{
	for (auto& collider : m_colliders)
	{
		collider->DrawDebug();
	}
}
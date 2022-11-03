#include "Physics/PhysicsSystem.hpp"

#include "EngineContext.hpp"
#include "Resources/SceneManager.hpp"

#include "ECS/Physics/Rigidbody.hpp"
#include "ECS/Physics/Collider.hpp"
#include "ECS/Physics/TerrainCollider.hpp"

#include "Core/Logger.hpp"

PhysicsSystem::PhysicsSystem()
{
	
}

PhysicsSystem::~PhysicsSystem()
{
	StopPhysics();
}


void PhysicsSystem::UpdateActor(PhysicsActor& actor)
{
	if (!running)
		return;

	PxRigidActor* pxActor = actor.GetRigidActor();

	if (pxActor) // If we already have an actor, we have to full recreate it (no other good choice)
	{
		m_simulation.RemoveActorFromSimulation(*pxActor);
		actor.RemoveFromSimulation();
	}
	
	actor.PrepareForSimulation();
	m_simulation.AddActorToSimulation(*actor.GetRigidActor());
}

void PhysicsSystem::RemoveActor(PhysicsActor& actor)
{
	for (int i = 0; i < m_actors.size(); ++i)
	{
		PhysicsActor* pa = m_actors[i].get();
		if (pa == &actor)
		{
			if (pa->GetRigidActor())
				m_simulation.RemoveActorFromSimulation(*pa->GetRigidActor());

			m_actors.erase(m_actors.begin() + i);
			return;
		}
	}
}

PhysicsActor* PhysicsSystem::FindActor(GameObject& gameObject)
{
	for (auto& a : m_actors)
	{
		PhysicsActor* actor = a.get();
		if (&actor->GetGameObject() == &gameObject)
			return actor;
	}

	return nullptr;
}

void PhysicsSystem::SendTransforms()
{
	for (auto& actor : m_actors)
	{
		if (!actor->GetGameObject().isStatic)
			actor->SendTransform();
	}
}

void PhysicsSystem::ReceiveTransforms()
{
	for (auto& actor : m_actors)
	{
		if (!actor->GetGameObject().isStatic)
			actor->RetreiveTransform();
	}
}

void PhysicsSystem::FixedUpdate(float fixedDeltaTime)
{
	m_simulation.StepSimulation(fixedDeltaTime);
}

void PhysicsSystem::StartPhysics()
{
	if (running)
		return;

	Logger::Info("Create physics simulation");

	running = true;

	CreateSimulation();

	for (auto& actor : m_actors)
	{
		actor->PrepareForSimulation();
		m_simulation.AddActorToSimulation(*(actor.get())->GetRigidActor());
	}
}

void PhysicsSystem::StopPhysics()
{
	if (!running)
		return;

	running = false;

	m_actors.clear();

	DestroySimulation();
}

void PhysicsSystem::CreateSimulation()
{
	m_simulation.Create();
}

void PhysicsSystem::DestroySimulation()
{
	m_simulation.Destroy();
}

PhysicsSimulation& PhysicsSystem::GetSimulation()
{
	return m_simulation;
}

bool PhysicsSystem::RaycastIgnoreSelf(const Vector3& origin, const Vector3& unitDirection, float maxDistance, PxRaycastHit& outHit, PxActor* selfActor, PxU32 layerMask)
{
	static PxRaycastHit hits[16];
	static PxRaycastBuffer hitsBuffer = PxRaycastBuffer(hits, 16);

	SystemManager::GetPhysicsSystem().RaycastLaunch(origin, unitDirection, maxDistance, hitsBuffer, UINT32_MAX);
	int hitNb = hitsBuffer.getNbAnyHits();

	bool result = false;;
	float minDistance = maxDistance * 2.0f;

	for (int i = hitNb - 1; i >= 0; i--)
	{
		float dist = hitsBuffer.getAnyHit(i).distance;

		if (dist < minDistance && hitsBuffer.getAnyHit(i).actor != selfActor)
		{
			minDistance = dist;
			outHit = hitsBuffer.getAnyHit(i);
			result = true;
		}
	}

	return result;
}

bool PhysicsSystem::Raycast(const Vector3& origin, const Vector3& unitDirection, float maxDistance, PxRaycastBuffer& outHit, PxU32 layerMask)
{
	return SystemManager::GetPhysicsSystem().RaycastLaunch(origin, unitDirection, maxDistance, outHit, layerMask);
}

bool PhysicsSystem::RaycastLaunch(const Vector3& origin, const Vector3& unitDirection, float maxDistance, PxRaycastBuffer& outHit, PxU32 layerMask)
{
	return m_simulation.Raycast(origin, unitDirection, maxDistance, outHit, layerMask);
}

void PhysicsSystem::DrawDebug()
{
	for (auto& actor : m_actors)
	{
		actor.get()->DrawDebug();
	}
}

Collider* PhysicsSystem::AddColliderInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id)
{
	PhysicsActor* actor = FindActor(owner);
	if (actor == nullptr) // If you don't find one (by gameObject identification), we create a new one
		actor = m_actors.emplace_back(std::make_unique<PhysicsActor>(owner, m_simulation)).get();

	Collider& collider = *m_colliders.emplace_back(archetype.makeUniqueInstance<Collider>(owner, id));
	actor->AddCollider(collider);

	UpdateActor(*actor);

	return &collider;
}

void PhysicsSystem::RemoveColliderInstance(Collider& collider)
{
	PhysicsActor* actor = FindActor(collider.gameObject);
	if (actor)
	{
		actor->RemoveShapes();
		actor->RemoveCollider(collider);

		if (actor->IsEmpty())
			RemoveActor(*actor);
		else
			UpdateActor(*actor);
	}

	// Find the the behavior
	auto it = std::find_if(m_colliders.begin(), m_colliders.end(),
		[&collider](std::unique_ptr<Collider>& _collider) { return _collider.get() == &collider; });
	
	// Should not happen
	if (it == m_colliders.end())
	{
		Logger::Error("PhysicsSystem - The collider you tried to remove not exists");
		return;
	}

	m_colliders.erase(it);
}

Rigidbody* PhysicsSystem::AddRigidbodyInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id)
{
	PhysicsActor* actor = FindActor(owner);
	if (actor == nullptr) // If you don't find one (by gameObject identification), we create a new one
		actor = m_actors.emplace_back(std::make_unique<PhysicsActor>(owner, m_simulation)).get();

	Rigidbody& rb = *m_rigidbodies.emplace_back(archetype.makeUniqueInstance<Rigidbody>(owner, id));
	actor->AddRigidbody(rb);

	UpdateActor(*actor);

	return &rb;
}

void PhysicsSystem::RemoveRigidbodyInstance(Rigidbody& rigidbody)
{
	PhysicsActor* actor = FindActor(rigidbody.gameObject);
	if (actor)
	{
		actor->RemoveRigidbody();

		if (actor->IsEmpty())
			RemoveActor(*actor);
		else
			UpdateActor(*actor);
	}

	// Find the the behavior
	auto it = std::find_if(m_rigidbodies.begin(), m_rigidbodies.end(),
		[&rigidbody](std::unique_ptr<Rigidbody>& _rigidbody) { return _rigidbody.get() == &rigidbody; });

	// Should not happen
	if (it == m_rigidbodies.end())
	{
		Logger::Error("PhysicsSystem - The collider you tried to remove not exists");
		return;
	}

	m_rigidbodies.erase(it);
}
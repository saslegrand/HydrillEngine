#include "ECS/Systems/SystemManager.hpp"

#include <Refureku/TypeInfo/Archetypes/Struct.h>
#include <miniaudio/miniaudio.h>

#include "ECS/Systems/BehaviorSystem.hpp"
#include "ECS/Systems/ParticleSystem.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/SoundSystem.hpp"
#include "ECS/Systems/MeshSystem.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Physics/PhysicsSystem.hpp"

#include "ECS/Behavior.hpp"
#include "ECS/Component.hpp"
#include "ECS/GameObject.hpp"
#include "ECS/MeshComponent.hpp"
#include "ECS/LightComponent.hpp"
#include "ECS/CameraComponent.hpp"
#include "ECS/Physics/Collider.hpp"
#include "ECS/ParticleComponent.hpp"
#include "ECS/Physics/Rigidbody.hpp"
#include "ECS/Sound/SoundEmitter.hpp"
#include "ECS/Sound/SoundListener.hpp"
#include "ECS/SkeletalMeshComponent.hpp"

#include "Core/Time.hpp"
#include "Core/Logger.hpp"

struct PimplSystems
{
	RenderSystem	m_renderSystem;
	BehaviorSystem	m_behaviorSystem;
	CameraSystem	m_cameraSystem;
	SoundSystem		m_soundSystem;
	MeshSystem		m_meshSystem;
	PhysicsSystem	m_physicsSystem;
	ParticleSystem	m_particleSystem;
};

SystemManager::SystemManager()
{
	m_pimpl = std::make_unique<PimplSystems>();
}

SystemManager::~SystemManager()
{

}

void SystemManager::Initialize()
{
	m_pimpl->m_physicsSystem.CreateSimulation();
	m_pimpl->m_meshSystem.Initialize();
}

void SystemManager::Start()
{
	m_pimpl->m_behaviorSystem.StartSystem();
	m_pimpl->m_soundSystem.StartSystem();
	m_pimpl->m_particleSystem.StartSystem();
	m_pimpl->m_physicsSystem.StartPhysics();
}

void SystemManager::Stop()
{
	m_pimpl->m_physicsSystem.StopPhysics();
	m_pimpl->m_behaviorSystem.StopSystem();
	m_pimpl->m_soundSystem.StopSystem();
	m_pimpl->m_particleSystem.StopSystem();
}

void SystemManager::FrameStart()
{
	m_pimpl->m_renderSystem.debugRenderPipeline.ClearPrimitives();
}

void SystemManager::PermanentUpdate()
{
	m_pimpl->m_meshSystem.Update();
	m_pimpl->m_cameraSystem.Update();
}

void SystemManager::Update(const Time& time)
{
	float deltatime = time.GetDeltaTime();
	float fixedDeltatime = time.GetFixedDeltaTime();

	// ======

	m_pimpl->m_physicsSystem.SendTransforms();

	static float stocker = 0;
	stocker += deltatime;
	while (stocker >= fixedDeltatime)
	{
		// Fixed Update
		// ============
		m_pimpl->m_behaviorSystem.FixedUpdateAll(fixedDeltatime);
		m_pimpl->m_particleSystem.FixedUpdateAll(fixedDeltatime);
		m_pimpl->m_physicsSystem.FixedUpdate(fixedDeltatime);

		// ============

		stocker -= fixedDeltatime;
	}

	m_pimpl->m_physicsSystem.ReceiveTransforms();

	m_pimpl->m_behaviorSystem.UpdateAll(deltatime);
	m_pimpl->m_particleSystem.UpdateAll(deltatime);
	m_pimpl->m_soundSystem.Update();
}

void SystemManager::LateUpdate()
{
	m_pimpl->m_behaviorSystem.LateUpdateAll();
}

Component* SystemManager::CreateComponent(rfk::Class const& compClass, GameObject& owner, const HYGUID& id)
{
	// Behaviors
	if (Behavior::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_behaviorSystem.AddBehaviorInstance(compClass, owner, id);

	// Rendering components
	if (MeshComponent::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_meshSystem.AddMeshInstance(compClass, owner, id);
	if (SkeletalMeshComponent::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_meshSystem.AddSkMeshInstance(compClass, owner, id);
	if (LightComponent::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_renderSystem.AddLightInstance(compClass, owner, id);
	if (CameraComponent::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_cameraSystem.AddCameraInstance(compClass, owner, id);
	if (ParticleComponent::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_particleSystem.AddParticleInstance(compClass, owner, id);

	// Physic components
	if (compClass.isSubclassOf(Collider::staticGetArchetype()))
		return m_pimpl->m_physicsSystem.AddColliderInstance(compClass, owner, id);
	if (Rigidbody::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_physicsSystem.AddRigidbodyInstance(compClass, owner, id);

	// Sound components
	if (SoundEmitter::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_soundSystem.AddEmitterInstance(compClass, owner, id);
	if (SoundListener::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_soundSystem.AddListenerInstance(compClass, owner, id);

	Logger::Error("SystemManager - Component '" + std::string(compClass.getName()) + "' is not attached to a system");
	return nullptr;
}

void SystemManager::DestroyComponent(Component& comp)
{
	rfk::Class const& compClass = comp.getArchetype();

	// Behaviors
	if (Behavior::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_behaviorSystem.RemoveBehaviorInstance(*static_cast<Behavior*>(&comp));

	// Rendering components
	if (MeshComponent::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_meshSystem.RemoveMeshInstance(*static_cast<MeshComponent*>(&comp));
	if (SkeletalMeshComponent::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_meshSystem.RemoveSkMeshInstance(*static_cast<SkeletalMeshComponent*>(&comp));
	if (LightComponent::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_renderSystem.RemoveLightInstance(*static_cast<LightComponent*>(&comp));
	if (CameraComponent::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_cameraSystem.RemoveCameraInstance(*static_cast<CameraComponent*>(&comp));
	if (ParticleComponent::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_particleSystem.RemoveParticleInstance(*static_cast<ParticleComponent*>(&comp));

	// Physic components
	if (compClass.isSubclassOf(Collider::staticGetArchetype()))
		return m_pimpl->m_physicsSystem.RemoveColliderInstance(*static_cast<Collider*>(&comp));
	if (Rigidbody::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_physicsSystem.RemoveRigidbodyInstance(*static_cast<Rigidbody*>(&comp));

	// Sound components
	if (SoundEmitter::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_soundSystem.RemoveEmitterInstance(*static_cast<SoundEmitter*>(&comp));
	if (SoundListener::staticGetArchetype().isBaseOf(compClass))
		return m_pimpl->m_soundSystem.RemoveListenerInstance(*static_cast<SoundListener*>(&comp));

	Logger::Error("SystemManager - Component '" + std::string(compClass.getName()) + "' is not attached to a system");
}

BehaviorSystem& SystemManager::GetBehaviorSystem()
{
	return EngineContext::Instance().systemManager->m_pimpl->m_behaviorSystem;
}

CameraSystem& SystemManager::GetCameraSystem()
{
	return EngineContext::Instance().systemManager->m_pimpl->m_cameraSystem;
}

SoundSystem& SystemManager::GetSoundSystem()
{
	return EngineContext::Instance().systemManager->m_pimpl->m_soundSystem;
}

MeshSystem& SystemManager::GetMeshSystem()
{
	return EngineContext::Instance().systemManager->m_pimpl->m_meshSystem;
}

PhysicsSystem& SystemManager::GetPhysicsSystem()
{
	return EngineContext::Instance().systemManager->m_pimpl->m_physicsSystem;
}

ParticleSystem& SystemManager::GetParticleSystem()
{
	return EngineContext::Instance().systemManager->m_pimpl->m_particleSystem;
}

RenderSystem& SystemManager::GetRenderSystem()
{
	SystemManager* s = EngineContext::Instance().systemManager;
	return s->m_pimpl->m_renderSystem;
}

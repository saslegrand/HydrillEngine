#include "ECS/Systems/ParticleSystem.hpp"

#include "ECS/Systems/SystemManager.hpp"

#include "ECS/ParticleComponent.hpp"
#include "ECS/GameObject.hpp"
#include "Renderer/RenderSystem.hpp"
#include "EngineContext.hpp"


void ParticleSystem::StartSystem()
{
	m_shouldInitiateParticle = true;

	AwakeAll();
	StartAll();
}

void ParticleSystem::StopSystem()
{
	StopAll();
	m_shouldInitiateParticle = false;
}

void ParticleSystem::InitializeNewParticles()
{
	if (!m_addParticle)
		return;

	m_addParticle = false;

	// Call Awake on behaviors added in game
	m_awakeInitialize.Invoke();
	m_awakeInitialize.ClearCallbacks();

	// Call Start on behaviors added in game
	m_startInitialize.Invoke();
	m_startInitialize.ClearCallbacks();
}

const std::vector<std::unique_ptr<ParticleComponent>>& ParticleSystem::GetAllParticleComponents() const
{
	return m_particleComponents;
}

void ParticleSystem::AwakeAll()
{
	for (auto& particleComp : m_particleComponents)
		if (particleComp->IsActive())
			particleComp->Awake();
}

void ParticleSystem::StartAll()
{
	for (auto& particleComp : m_particleComponents)
	{
		if (particleComp->IsActive())
			particleComp->Start();
	}
}

void ParticleSystem::StopAll()
{
	for (auto& particleComp : m_particleComponents)
	{
		if (particleComp->IsActive())
			particleComp->Stop();
	}
}

void ParticleSystem::UpdateAll(float tick)
{
	InitializeNewParticles();

	for (auto& particleComp : m_particleComponents)
	{
		if (particleComp->IsActive())
			particleComp->Update(tick);
	}
}

void ParticleSystem::FixedUpdateAll(float fixedTick)
{
	for (auto& particleComp : m_particleComponents)
	{
		if (particleComp->IsActive())
			particleComp->FixedUpdate(fixedTick);
	}
}

ParticleComponent* ParticleSystem::AddParticleInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id)
{
	ParticleComponent& particleComp = *m_particleComponents.emplace_back(archetype.makeUniqueInstance<ParticleComponent>(owner, id));

	SystemManager::GetRenderSystem().Register(&particleComp);

	if (m_shouldInitiateParticle)
	{
		m_addParticle = true;
		m_awakeInitialize.AddCallback(&ParticleComponent::Awake, particleComp);
		m_startInitialize.AddCallback(&ParticleComponent::Start, particleComp);
	}

	return &particleComp;
}

void ParticleSystem::RemoveParticleInstance(ParticleComponent& particleComp)
{
	auto it = std::find_if(m_particleComponents.begin(), m_particleComponents.end(),
		[&particleComp](std::unique_ptr<ParticleComponent>& _particleComp) { return _particleComp.get() == &particleComp; });

	if (it == m_particleComponents.end())
	{
		Logger::Error("ParticleSystem - The particle component you tried to remove not exists");
		return;
	}

	SystemManager::GetRenderSystem().Unregister(&particleComp);
	m_particleComponents.erase(it);
}
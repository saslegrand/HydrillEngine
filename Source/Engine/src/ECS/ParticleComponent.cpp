
#include "Renderer/MeshRenderPipeline.hpp"
#include "Renderer/RenderSystem.hpp"
#include "EngineContext.hpp"

#include "Resources/SceneManager.hpp"
#include "ECS/Systems/ParticleSystem.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/GameObject.hpp"

#include "Generated/ParticleComponent.rfks.h"


rfk::UniquePtr<ParticleComponent> ParticleComponent::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<ParticleComponent>(GO, uid);
}



ParticleComponent::ParticleComponent(GameObject& gameObject, const HYGUID& uid)
	: Component(gameObject, getArchetype().getName(), uid)
{
	m_isUnique = true;
}


ParticleComponent::~ParticleComponent()
{
	emitters.clear();
}


void ParticleComponent::Initialize()
{
	//	Initialize and generate emitters
	for (ParticleEmitter& emitter : emitters)
	{
		emitter.Initialize(gameObject);
	}
}


void ParticleComponent::Awake()
{
}

void ParticleComponent::Start()
{
	m_playing = true;

	for (ParticleEmitter& emitter : emitters)
	{
		emitter.Start();
	}
}

void ParticleComponent::Stop()
{
	m_playing = false;

	for (ParticleEmitter& emitter : emitters)
	{
		emitter.Stop();
	}
}

void ParticleComponent::ForceStop()
{
	m_playing = false;

	for (ParticleEmitter& emitter : emitters)
	{
		emitter.Clear();
	}
}



void ParticleComponent::Update(float tick)
{
	m_elapsedTime += tick;

	if (m_playing)
	{
		if (m_elapsedTime >= duration)
		{
			m_playing = false;
			m_elapsedTime = 0.0f;
			Stop();
		}
	}
	else if (!m_playing && loop)
	{
		if (m_elapsedTime >= gapDuration)
		{
			m_playing = true;
			m_elapsedTime = 0.0f;
			Start();
		}
	}
	else
	{
		bool finished = true;

		for (ParticleEmitter& emitter : emitters)
		{
			if (!(emitter.state == ParticleEmitter::EmitterState::Finished || emitter.state == ParticleEmitter::EmitterState::Inactive))
			{
				finished = false;
			}
		}

		if (finished)
		{
			SetActive(false);

			if (EngineContext::Instance().playing && autoDestroyGameObject) SceneManager::DestroyGameObject(gameObject);
		}
	}

	for (ParticleEmitter& emitter : emitters)
	{
		if (emitter.state != ParticleEmitter::EmitterState::Inactive)
		{
			emitter.gameObject = &gameObject;

			emitter.Update(tick);
		}
	}
}

void ParticleComponent::FixedUpdate(float fixedTick)
{
	for (ParticleEmitter& emitter : emitters)
	{
		if (emitter.state != ParticleEmitter::EmitterState::Inactive) emitter.FixedUpdate(fixedTick);
	}
}


void ParticleComponent::OnModify()
{
	//	If an emitter was added then pass gameobject reference to it
	if (m_emittercount < emitters.size())
	{
		emitters.back().Initialize(gameObject);
	}
}


void ParticleComponent::SetActive(bool value)
{
	SceneObject::SetActive(value);

	if (!value)
	{
		ForceStop();
	}
	else
	{
		Start();
	}
}
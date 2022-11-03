#include "ECS/GameObject.hpp"
#include "Renderer/RenderSystem.hpp"
#include "EngineContext.hpp"

#include "Generated/ParticleEmitter.rfks.h"


void ParticleTexture::GenerateTexture()
{
	RenderSystem& render = SystemManager::GetRenderSystem();

	if (texture) render.GPUWrapper.CopyInTextureArray(*texture);
}

ParticleEmitter::ParticleEmitter()
{
	//	Manual test here
	//	This values must be reflected, editable and serialized later
	parameters.lifeStart.valueMin = 1.f;
	parameters.lifeStart.valueMax = 2.f;

	parameters.burstForce.valueMin = 500.f;
	parameters.burstForce.valueMax = 750.f;

	parameters.sizeScale.curveType = EasingCurve::EaseInOutCirc;
	parameters.sizeScale.value = 1.f;

	parameters.airDragging.value = 0.05f;
}

ParticleEmitter::~ParticleEmitter()
{

}

void ParticleEmitter::ResizeInstanceList()
{
	size_t newSize = static_cast<size_t>(maxInstance);

	instances.resize(newSize, *this);
}


void ParticleEmitter::SpawnParticles(float tick)
{
	float duration = spawnDuration;

	if (particleLoop)
	{
		//	If particle loop is enabled ignore spawnduration and use max life instead
		duration = parameters.lifeStart.valueMax;
	}

	elapsedTimeSpawning += tick;
	elapsedTimeSinceLastBurst += tick;

	float spawnGap = duration / static_cast<float>(maxInstance);
	if (spawnGap < elapsedTimeSinceLastBurst)
	{
		unsigned int numToSpawn = spawnGap > 0.0f ? static_cast<int>(elapsedTimeSinceLastBurst / spawnGap) : maxInstance;

		for (unsigned int i = 0; i < numToSpawn; i++)
		{
			if (spawnIndex >= maxInstance)
			{
				state = EmitterState::Playing;

				elapsedTimeSpawning = 0.0f;
				elapsedTimeSinceLastBurst = 0.0f;

				return;
			}

			instances[spawnIndex].Spawn(*this);
			spawnIndex++;
		}
		elapsedTimeSinceLastBurst = 0.0f;
	}

	if (elapsedTimeSpawning >= duration)
	{
		state = EmitterState::Playing;

		elapsedTimeSpawning = 0.0f;
		elapsedTimeSinceLastBurst = 0.0f;
	}
}


void ParticleEmitter::Initialize(GameObject& parentGameObject)
{
	gameObject = &parentGameObject;
	diffuseTexture.GenerateTexture();
	emissiveTexture.GenerateTexture();
	blendTexture01.GenerateTexture();
	blendTexture02.GenerateTexture();
	blendTexture03.GenerateTexture();
	blendTexture04.GenerateTexture();
}


void ParticleEmitter::Update(float tick)
{
	//	Check if resize is needed
	if (maxInstance != instances.size())
	{
		ResizeInstanceList();
	}
	
	//	Check if all particles are dead or not
	bool isFinished = true;

	if (state == EmitterState::Spawning)
	{
		SpawnParticles(tick);
		isFinished = false;
	}

	//	Update all particle instances
	for (ParticleInstance& instance : instances)
	{
		//	Only update if the instance is active
		if (instance.IsAlive())
		{
			//	If at least one particle is alive, then the emitter is still active
			isFinished = false;
			instance.Update(*this, tick);
		}
	}

	if (isFinished)
	{
		state = EmitterState::Finished;
	}
}


void ParticleEmitter::FixedUpdate(float fixedTick)
{
	for (ParticleInstance& instance : instances)
	{
		if (instance.IsAlive()) instance.FixedUpdate(*this, fixedTick);
	}
}


void ParticleEmitter::Start()
{
	//	Check if a resize is needed
	if (static_cast<int>(instances.size()) != maxInstance) ResizeInstanceList();

	state = EmitterState::Spawning;
	allowParticleRespawnOnDeath = particleLoop;
	elapsedTimeSinceLastBurst = 0.0f;
	elapsedTimeSpawning = 0.0f;
	spawnIndex = 0;
}

void ParticleEmitter::Stop()
{
	//	Only stop spawning
	state = EmitterState::Playing;
	allowParticleRespawnOnDeath = false;
}

void ParticleEmitter::Clear()
{
	instances.clear();
}

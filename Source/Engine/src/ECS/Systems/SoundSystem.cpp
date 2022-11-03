#include "ECS/Systems/SoundSystem.hpp"

#include <miniaudio/miniaudio.h>

#include "ECS/GameObject.hpp"
#include "ECS/Sound/SoundEmitter.hpp"
#include "ECS/Sound/SoundListener.hpp"
#include "EngineContext.hpp"
#include "Core/Logger.hpp"
#include "ECS/Physics/Rigidbody.hpp"
#include "Resources/Resource/Sound.hpp"

#include "Generated/SoundEnums.rfks.h"

SoundSystem::SoundSystem()
{
	// Create the sound engine
	m_soundEngine = std::make_unique<ma_engine>();

	// Initialize the sound engine
	ma_result engineResult = ma_engine_init(NULL, m_soundEngine.get());
	if (engineResult != MA_SUCCESS) 
	{
		Logger::Error("SoundSystem - Failed to initialize the sound engine");
		return;
	}
}

SoundSystem::~SoundSystem()
{
	// Destroy the loaded sounds
	for (Sound* sound : m_sounds)
		sound->EraseSoundData();

	// Destroy the sound engine
	ma_engine_uninit(m_soundEngine.get());
}

void SoundSystem::StartSystem()
{
	SoundSystem& self = SystemManager::GetSoundSystem();

	self.m_systemStarted = true;

	// Start emitters
	for (auto& emitter : self.m_emitters)
	{
		emitter->Start();

		// Set emitter position and direction (if static, not updated)
		Transform& transform = emitter->gameObject.transform;

		emitter->SetPosition(transform.Position());
		emitter->SetDirection(transform.Forward());
	}
}

void SoundSystem::StopSystem()
{
	SoundSystem& self = SystemManager::GetSoundSystem();

	self.m_systemStarted = false;

	// Stop all sounds
	for (auto& emitter : self.m_emitters)
		emitter->Stop();
}

void SoundSystem::Update()
{
	// Start emitters added in game then remove from event
	m_eNewSoundStart.Invoke();
	m_eNewSoundStart.ClearCallbacks();

	// Update emitters
	for (auto& emitter : m_emitters)
	{
		if (!emitter->IsActive())
			return;

		if (emitter->gameObject.isStatic || !emitter->IsSpatialized())
			continue;

		// Set emitter position and direction
		Transform& transform = emitter->gameObject.transform;

		emitter->SetPosition(transform.Position());
		emitter->SetDirection(transform.Forward());

		// Set the emitter velocity
		Rigidbody* emitterRb = nullptr;
		if (emitter->gameObject.TryGetComponent<Rigidbody>(&emitterRb))
			emitter->SetVelocity(emitterRb->GetVelocity());
	}

	// Try update listener
	if (m_activeListener == nullptr)
		return;

	m_activeListener->SetPosition(m_activeListener->gameObject.transform.Position());
	m_activeListener->SetDirection(m_activeListener->gameObject.transform.Forward());

	Rigidbody* listenerRb = nullptr;
	if (m_activeListener->gameObject.TryGetComponent<Rigidbody>(&listenerRb))
		m_activeListener->SetVelocity(listenerRb->GetVelocity());
}

void SoundSystem::ChangeActiveListener(SoundListener& listener)
{
	SoundSystem& self = SystemManager::GetSoundSystem();
	self.m_activeListener = &listener;
}

const std::vector<std::unique_ptr<SoundEmitter>>& SoundSystem::GetAllSoundEmitters()
{
	SoundSystem& self = SystemManager::GetSoundSystem();
	return self.m_emitters;
}

const std::vector<std::unique_ptr<SoundListener>>& SoundSystem::GetAllSoundListeners()
{
	SoundSystem& self = SystemManager::GetSoundSystem();
	return self.m_listeners;
}

ma_engine& SoundSystem::GetSoundEngine() const
{
	return *m_soundEngine;
}

void SoundSystem::RegisterSound(Sound* sound)
{
	m_sounds.emplace_back(sound);
}

#pragma region REGISTRATION

SoundEmitter* SoundSystem::AddEmitterInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id)
{
	SoundEmitter& emitter = *m_emitters.emplace_back(archetype.makeUniqueInstance<SoundEmitter>(owner, id));

	if (m_systemStarted)
		m_eNewSoundStart.AddCallback(&SoundEmitter::Start, emitter);

	return &emitter;
}

void SoundSystem::RemoveEmitterInstance(SoundEmitter& emitter)
{
	auto it = std::find_if(m_emitters.begin(), m_emitters.end(),
		[&emitter](std::unique_ptr<SoundEmitter>& _emitter) { return _emitter.get() == &emitter; });

	if (it == m_emitters.end())
	{
		Logger::Error("SoundSystem - The sound emitter you tried to remove not exists");
		return;
	}

	m_emitters.erase(it);
}

SoundListener* SoundSystem::AddListenerInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id)
{
	bool shouldSetListenerActive = (m_listeners.size() == 0) && (m_activeListener == nullptr);

	SoundListener& listener = *m_listeners.emplace_back(archetype.makeUniqueInstance<SoundListener>(owner, id));

	// Switch to component listener
	if (shouldSetListenerActive)
		ChangeActiveListener(listener);

	listener.Init(*m_soundEngine);

	return &listener;
}

void SoundSystem::RemoveListenerInstance(SoundListener& listener)
{
	auto it = std::find_if(m_listeners.begin(), m_listeners.end(),
		[&listener](std::unique_ptr<SoundListener>& _listener) { return _listener.get() == &listener; });

	if (it == m_listeners.end())
	{
		Logger::Error("SoundSystem - The sound listener you tried to remove not exists");
		return;
	}

	m_listeners.erase(it);

	// Switch to global listener
	if (m_listeners.size() == 0)
		m_activeListener = nullptr;
}

#pragma endregion
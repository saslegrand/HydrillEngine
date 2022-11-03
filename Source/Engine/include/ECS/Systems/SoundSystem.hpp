#pragma once

#include <vector>
#include <memory>

#include <Refureku/TypeInfo/Archetypes/Struct.h>

#include "EngineDll.hpp"
#include "Tools/Event.hpp"

class SoundEmitter;
class SoundListener;
class Sound;
class GameObject;
class HYGUID;

struct ma_engine;

/**
@brief System that handle every behaviors in the game
*/
class SoundSystem
{
private:
	std::unique_ptr<ma_engine> m_soundEngine;

	std::vector<std::unique_ptr<SoundEmitter>>	m_emitters;
	std::vector<std::unique_ptr<SoundListener>> m_listeners;
	std::vector<Sound*>							m_sounds;

	SoundListener* m_activeListener = nullptr;

	bool m_systemStarted = false;

	Event<> m_eNewSoundStart;

public:
	SoundSystem();
	~SoundSystem();

public:
	/**
	@brief Start the system and the attached emitters
	*/
	static ENGINE_API void StartSystem();

	/*
	@brief Stop the system
	*/
	static ENGINE_API void StopSystem();

	/**
	@brief Set a new active listener

	@param listener : New active listener
	*/
	static ENGINE_API void ChangeActiveListener(SoundListener& listener);

	/**
	@brief Get all sound emitters

	@return const std::vector<SoundEmitter*>& list of sound emitter
	*/
	static ENGINE_API const std::vector<std::unique_ptr<SoundEmitter>>& GetAllSoundEmitters();

	/**
	@brief Get all sound listeners

	@return const std::vector<SoundListener*>& list of sound listener
	*/
	static ENGINE_API const std::vector<std::unique_ptr<SoundListener>>& GetAllSoundListeners();

	/**
	@brief Add a sound reference to the system (delete the sound before the sound engine)

	@param sound : Sound reference
	*/
	void RegisterSound(Sound* sound);

	/**
	@brief Return the current sound engine

	@return ma_engine : Sound engine
	*/
	ma_engine& GetSoundEngine() const;

	/**
	System update call on game update
	*/
	void Update();

	/**
	@brief Add a new sound emitter to the system

	@param owner : Owner of the sound emitter
	@param id : Unique identifier of the sound emitter

	@return Behavior* : Created sound emitter, nullptr if not valid
	*/
	SoundEmitter* AddEmitterInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id);

	/**
	@brief Remove an existing sound emitter in the system

	@param emitter : sound emitter to remove
	*/
	void RemoveEmitterInstance(SoundEmitter& emitter);

	/**
	@brief Add a new sound listener to the system

	@param owner : Owner of the sound listener
	@param id : Unique identifier of the sound listener

	@return Behavior* : Created sound listener, nullptr if not valid
	*/
	SoundListener* AddListenerInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id);

	/**
	@brief Remove an existing sound listener in the system

	@param listener : sound listener to remove
	*/
	void RemoveListenerInstance(SoundListener& listener);
};
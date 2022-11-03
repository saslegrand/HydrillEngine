#include "EngineContext.hpp"

#include <miniaudio/miniaudio.h>

#include "ECS/Systems/SoundSystem.hpp"

#include "Generated/SoundListener.rfks.h"


rfk::UniquePtr<SoundListener> SoundListener::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<SoundListener>(GO, uid);
}


SoundListener::SoundListener(GameObject& gameObject, const HYGUID& uid)
	: Component(gameObject, getArchetype().getName(), uid)
{
	m_isUnique = true;

	//EngineContext::Instance().soundSystem->Register(*this);

	// TODO Check for listener attenuation parameters
}

SoundListener::~SoundListener()
{
	//EngineContext::Instance().soundSystem->Unregister(*this);
}

void SoundListener::Init(ma_engine& soundEngine)
{
	m_soundEngine = &soundEngine;
}

void SoundListener::SetDirection(const Vector3& newDirection)
{
	ma_engine_listener_set_direction(m_soundEngine, 0, newDirection.x, newDirection.y, newDirection.z);
}

void SoundListener::SetPosition(const Vector3& newPosition)
{
	ma_engine_listener_set_position(m_soundEngine, 0, newPosition.x, newPosition.y, newPosition.z);
}

void SoundListener::SetVelocity(const Vector3& newVelocity)
{
	ma_engine_listener_set_velocity(m_soundEngine, 0, newVelocity.x, newVelocity.y, newVelocity.z);
}
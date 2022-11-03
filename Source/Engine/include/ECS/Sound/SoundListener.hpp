#pragma once

#include "ECS/Gameobject.hpp"

#include "Generated/SoundListener.rfkh.h"

struct ma_engine;

/**
* TODO
*/
class HY_CLASS() ENGINE_API SoundListener : public Component
{
private:
	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<SoundListener> defaultInstantiator(GameObject & GO, const HYGUID & uid);

private:
	ma_engine* m_soundEngine = nullptr;

public:
	//	Constructors & Destructors

	SoundListener(GameObject& gameObject, const HYGUID& uid);
	SoundListener(SoundListener const& cam) = delete;
	SoundListener(SoundListener&& cam) = default;

	SoundListener& operator=(SoundListener const& cam) = delete;
	SoundListener& operator=(SoundListener && cam) = default;

	~SoundListener();

public:
	void Init(ma_engine& soundEngine);

	/**
	Set the listener new direction (for the directional attenuation)

	@param newVelocity : New velocity
	*/
	void SetDirection(const Vector3& newDirection);

	/**
	Set the listener new position (for spatialization)

	@param newVelocity : New velocity
	*/
	void SetPosition(const Vector3& newPosition);

	/**
	Set the listener new velocity (combined with doppler factor)

	@param newVelocity : New velocity
	*/
	void SetVelocity(const Vector3& newVelocity);

	SoundListener_GENERATED
};

File_SoundListener_GENERATED
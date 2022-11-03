#include <miniaudio/miniaudio.h>

#include "EngineContext.hpp"
#include "ECS/Systems/SoundSystem.hpp"
#include "Resources/Resource/Sound.hpp"

#include "Maths/Maths.hpp"

#include "Generated/SoundEmitter.rfks.h"


rfk::UniquePtr<SoundEmitter> SoundEmitter::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<SoundEmitter>(GO, uid);
	//ma_sound_get_current_fade_volume
}


SoundEmitter::SoundEmitter(GameObject& gameObject, const HYGUID& uid)
	: Component(gameObject, getArchetype().getName(), uid)
{
	m_isUnique = true;

	// Register to the sound system
	//EngineContext::Instance().soundSystem->Register(*this);

	// Construct the sound data isntance handle
	m_soundData = std::make_unique<ma_sound>();
}

SoundEmitter::~SoundEmitter()
{
	// Uninit instatiates sound
	if (m_instantiated)
		ma_sound_uninit(m_soundData.get());

	// Unregister from sound system
	//EngineContext::Instance().soundSystem->Unregister(*this);
}

void SoundEmitter::Start()
{
	// Force update all sound parameters
	UpdateSoundValues();
	PinListerner(0);

	// Play on start if activate
	if (m_playOnStart)
		Play();
}

void SoundEmitter::CreateSoundCopy()
{
	// Check if a sound resource is attached
	if (m_sound == nullptr)
		return;

	ma_engine& soundEngine = SystemManager::GetSoundSystem().GetSoundEngine();

	// Copy attached sound
	ma_result result;
	result = ma_sound_init_copy(&soundEngine, m_sound->GetSoundData(), MA_SOUND_FLAG_DECODE, nullptr, m_soundData.get());

	if (result != MA_SUCCESS)
	{
		Logger::Error("SoundEmitter - Failed to create sound instance");
		return;
	}

	// Set to ready
	m_instantiated = true;
}

void SoundEmitter::UpdateSoundValues()
{
	// Check if a sound resource is attached
	if (m_sound == nullptr)
		return;

	if (!m_instantiated)
		CreateSoundCopy();

	SetLooping(m_loop); 
	SetSpatialization(m_spatialized);

	SetLooping(m_loop);
	SetVolume(m_volume);
	SetRolloff(m_rolloff);
	SetDopplerFactor(m_dopplerFactor);
	SetPan(m_pan);
	SetPitch(m_pitch);
	SetGain(m_gain.x, m_gain.y);

	SetDirectionalAttenuationFactor(m_directionalAttenuation);
	SetDistance(m_attenuationDistance.x, m_attenuationDistance.y);
	SetAttenuationModel(m_attenuationModel);
	SetConeAngles(m_coneInOutAngles);
	SetConeOuterGain(m_coneOuterGain);
}

//void Attach_Detach()
//{
	//ma_node_attach_output_bus(&sound, 0, &soundGroup, 0);
	//ma_node_attach_output_bus(&soundGroup, 0, &soundGroup2, 0);
//}

void SoundEmitter::SetSound()
{
	SetSound(m_sound);
}

void SoundEmitter::SetSound(Sound* sound)
{
	// Check if the sound is not already set
	if (sound == nullptr)
		return;

	// Uninit previous instantiated sound
	if (m_instantiated)
		ma_sound_uninit(m_soundData.get());

	m_sound = sound;
	m_instantiated = false;

	UpdateSoundValues();
}

void SoundEmitter::SetPlayOnStart(bool shouldPlayOnStart)
{
	m_playOnStart = shouldPlayOnStart;
}

bool SoundEmitter::ShouldPlayOnStart() const
{
	return m_playOnStart;
}

void SoundEmitter::PinListerner(uint32_t listernerIndex)
{
	if (!m_instantiated)
		return;

	ma_sound_set_pinned_listener_index(m_soundData.get(), listernerIndex);
}

uint32_t SoundEmitter::GetPinnedListenerIndex() const
{
	if (!m_instantiated)
		return 0;

	return ma_sound_get_listener_index(m_soundData.get());
}

void SoundEmitter::SetLooping(bool isLooping)
{
	if (!m_instantiated)
		return;

	m_loop = isLooping;

	ma_sound_set_looping(m_soundData.get(), isLooping);
}

bool SoundEmitter::IsLooping() const
{
	return m_loop;
}

void SoundEmitter::SetSpatialization(bool isSpatialized)
{
	if (!m_instantiated)
		return;

	m_spatialized = isSpatialized;

	ma_sound_set_spatialization_enabled(m_soundData.get(), isSpatialized);
}

bool SoundEmitter::IsSpatialized() const
{
	return m_spatialized;
}

void SoundEmitter::SetConeAngles(Vector2 inOutAnglesInDegrees)
{
	if (!m_instantiated)
		return;

	m_coneInOutAngles = inOutAnglesInDegrees;

	Logger::Info("Set emitter cone angles -  InAngle : " + std::to_string(m_coneInOutAngles.x) + "OutAngle : " + std::to_string(m_coneInOutAngles.y));
	SetCone(m_coneInOutAngles.x, m_coneInOutAngles.y, m_coneOuterGain);
}

void SoundEmitter::SetConeOuterGain(float outerGain)
{
	if (!m_instantiated)
		return;

	m_coneOuterGain = outerGain;

	Logger::Info("Set emitter cone gain -  Gain : " + std::to_string(m_coneOuterGain));
	SetCone(m_coneInOutAngles.x, m_coneInOutAngles.y, m_coneOuterGain);
}

void SoundEmitter::SetCone(float innerAngleInDegrees, float outerAngleInDegrees, float outerGain)
{
	if (!m_instantiated)
		return;

	Logger::Info("Set emitter cone - InAngle : " + std::to_string(innerAngleInDegrees) + "OutAngle : " + std::to_string(outerAngleInDegrees) + "OuterGain : " + std::to_string(outerGain));

	m_coneInOutAngles = { innerAngleInDegrees, outerAngleInDegrees };
	m_coneOuterGain = outerGain;

	ma_sound_set_cone(m_soundData.get(), Maths::DEGTORAD * innerAngleInDegrees, Maths::DEGTORAD * outerAngleInDegrees, outerGain);
}

Vector2 SoundEmitter::GetConeAngles()
{
	return m_coneInOutAngles;
}

float SoundEmitter::GetConeOuterGain()
{
	return m_coneOuterGain;
}

void SoundEmitter::GetCone(float* innerAngleInDegrees, float* outerAngleInDegrees, float* outerGain)
{
	if (!m_instantiated)
		return;

	ma_sound_get_cone(m_soundData.get(), innerAngleInDegrees, outerAngleInDegrees, outerGain);
}


void SoundEmitter::SetDirectionalAttenuationFactor(float directionalAttenuationFactor)
{
	if (!m_instantiated)
		return;

	m_directionalAttenuation = directionalAttenuationFactor;

	ma_sound_set_directional_attenuation_factor(m_soundData.get(), directionalAttenuationFactor);
}

float SoundEmitter::GetDirectionalAttenuationFactor() const
{
	return m_directionalAttenuation;
}

void SoundEmitter::SetAttenuationModel(ESoundAttenuationModel attenuationModel)
{
	if (!m_instantiated)
		return;

	m_attenuationModel = attenuationModel;

	ma_sound_set_attenuation_model(m_soundData.get(), static_cast<ma_attenuation_model>(attenuationModel));
}

void SoundEmitter::SetAttenuationModel(int16_t attenuationModel)
{
	if (!m_instantiated)
		return;

	m_attenuationModel = static_cast<ESoundAttenuationModel>(attenuationModel);

	ma_sound_set_attenuation_model(m_soundData.get(), static_cast<ma_attenuation_model>(attenuationModel));
}

ESoundAttenuationModel SoundEmitter::GetAttenuationModel() const
{
	return m_attenuationModel;
}

void SoundEmitter::SetMinGain(float minGain)
{
	if (!m_instantiated)
		return;

	m_gain.x = minGain;

	ma_sound_set_min_gain(m_soundData.get(), minGain);
}

float SoundEmitter::GetMinGain() const
{
	return m_gain.x;
}

void SoundEmitter::SetMaxGain(float maxGain)
{
	if (!m_instantiated)
		return;

	m_gain.y = maxGain;

	ma_sound_set_max_gain(m_soundData.get(), maxGain);
}

float SoundEmitter::GetMaxGain() const
{
	return m_gain.y;
}

void SoundEmitter::SetGain(float minGain, float maxGain)
{
	if (!m_instantiated)
		return;

	m_gain = { minGain, maxGain };

	SetMinGain(minGain);
	SetMaxGain(maxGain);
}

void SoundEmitter::SetGain(Vector2 minMaxGain)
{
	if (!m_instantiated)
		return;

	m_gain = minMaxGain;

	SetMinGain(minMaxGain.x);
	SetMaxGain(minMaxGain.y);
}

void SoundEmitter::SetMinDistance(float minDistance)
{
	if (!m_instantiated)
		return;

	m_attenuationDistance.x = minDistance;

	ma_sound_set_min_distance(m_soundData.get(), minDistance);
}

float SoundEmitter::GetMinDistance() const
{
	return m_attenuationDistance.x;
}

void SoundEmitter::SetMaxDistance(float maxDistance)
{
	if (!m_instantiated)
		return;

	m_attenuationDistance.y = maxDistance;

	ma_sound_set_max_distance(m_soundData.get(), maxDistance);
}

float SoundEmitter::GetMaxDistance() const
{
	return m_attenuationDistance.y;
}

void SoundEmitter::SetDistance(float minDistance, float maxDistance)
{
	if (!m_instantiated)
		return;

	m_attenuationDistance = { minDistance, maxDistance };

	SetMinDistance(minDistance);
	SetMaxDistance(maxDistance);
}

void SoundEmitter::SetDistance(Vector2 minMaxDistance)
{
	if (!m_instantiated)
		return;

	m_attenuationDistance = minMaxDistance;

	SetMinDistance(minMaxDistance.x);
	SetMaxDistance(minMaxDistance.y);
}

Vector3 SoundEmitter::GetVelocity() const
{
	if (!m_instantiated)
		return Vector3::Zero;

	ma_vec3f vec3 = ma_sound_get_velocity(m_soundData.get());
	Vector3 velocity = Vector3(vec3.x, vec3.y, vec3.z);

	return velocity;
}

void SoundEmitter::SetVelocity(const Vector3& newVelocity)
{
	if (!m_instantiated)
		return;

	// Link to the doppler factor
	ma_sound_set_velocity(m_soundData.get(), newVelocity.x, newVelocity.y, newVelocity.z);
}

Vector3 SoundEmitter::GetDirection() const
{
	if (!m_instantiated)
		return Vector3::Zero;

	ma_vec3f vec3 = ma_sound_get_direction(m_soundData.get());
	Vector3 direction = Vector3(vec3.x, vec3.y, vec3.z);

	return direction;
}

void SoundEmitter::SetDirection(const Vector3& newDirection)
{
	if (!m_instantiated)
		return;

	// Link to the doppler factor
	ma_sound_set_direction(m_soundData.get(), newDirection.x, newDirection.y, newDirection.z);
}

Vector3 SoundEmitter::GetPosition() const
{
	if (!m_instantiated)
		return Vector3::Zero;

	ma_vec3f vec3 = ma_sound_get_position(m_soundData.get());
	Vector3 position = Vector3(vec3.x, vec3.y, vec3.z);

	return position;
}

void SoundEmitter::SetPosition(const Vector3& newPosition)
{
	if (m_sound == nullptr)
		return;

	// Link to the doppler factor
	ma_sound_set_position(m_soundData.get(), newPosition.x, newPosition.y, newPosition.z);
}

float SoundEmitter::GetRolloff() const
{
	return m_rolloff;
}

void SoundEmitter::SetRolloff(float newRolloff)
{
	if (!m_instantiated)
		return;

	m_rolloff = newRolloff;

	ma_sound_set_rolloff(m_soundData.get(), newRolloff);
}

float SoundEmitter::GetVolume() const
{
	return m_volume;
}

void SoundEmitter::SetVolume(float newVolume)
{
	if (!m_instantiated)
		return;

	m_volume = newVolume;

	ma_sound_set_volume(m_soundData.get(), newVolume);
}

float SoundEmitter::GetDopplerFactor() const
{
	return m_dopplerFactor;
}

void SoundEmitter::SetDopplerFactor(float newDopplerFactor)
{
	if (!m_instantiated)
		return;

	m_dopplerFactor = newDopplerFactor;

	ma_sound_set_doppler_factor(m_soundData.get(), newDopplerFactor);
}

float SoundEmitter::GetPan() const
{
	return m_pan;
}

void SoundEmitter::SetPan(float newPan)
{
	if (!m_instantiated)
		return;

	m_pan = newPan;

	ma_sound_set_pan(m_soundData.get(), newPan);
}

float SoundEmitter::GetPitch() const
{
	return m_pitch;
}

void SoundEmitter::SetPitch(float newPitch)
{
	if (!m_instantiated)
		return;

	m_pitch = newPitch;

	ma_sound_set_pitch(m_soundData.get(), newPitch);
}

void SoundEmitter::Fade(float beginVolume, float endVolume, uint64_t fadeTimeMilli)
{
	if (!m_instantiated)
		return;

	ma_sound_set_fade_in_milliseconds(m_soundData.get(), beginVolume, endVolume, fadeTimeMilli);
}

bool SoundEmitter::IsPlaying() const
{
	if (!m_instantiated)
		return false;

	return ma_sound_is_playing(m_soundData.get());
}

bool SoundEmitter::IsFinished() const
{
	if (!m_instantiated)
		return false;

	return ma_sound_at_end(m_soundData.get());
}

void SoundEmitter::Play()
{
	if (!m_instantiated)
		return;

	if (!IsPlaying())
		ma_sound_start(m_soundData.get());
}

void SoundEmitter::PlayFromStart()
{
	PlayFrom(0);
}

void SoundEmitter::PlayFrom(uint64_t pcmFrame)
{
	if (!m_instantiated)
		return;

	// Stop the sound if playing
	Stop();

	// Move cursor to the beginning
	ma_sound_seek_to_pcm_frame(m_soundData.get(), pcmFrame);

	// Play the sound
	Play();
}

void SoundEmitter::Stop()
{
	if (!m_instantiated)
		return;

	if (IsPlaying())
		ma_sound_stop(m_soundData.get());
}

void SoundEmitter::PlayWithDelay(uint64_t delayMilli)
{
	if (!m_instantiated)
		return;

	ma_sound_set_start_time_in_milliseconds(m_soundData.get(), delayMilli);
	Play();
}

void SoundEmitter::StopWithDelay(uint64_t delayMilli)
{
	if (!m_instantiated)
		return;

	ma_sound_set_stop_time_in_milliseconds(m_soundData.get(), delayMilli);
}
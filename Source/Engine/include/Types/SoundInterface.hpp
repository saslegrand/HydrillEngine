#pragma once

#include "Maths/Vector3.hpp"
#include "Tools/SoundEnums.hpp"

/**
@brief Serializable makes the class usable for file serialization
*/
class SoundInterface
{
#pragma region DE/ACTIVATE PARAMETERS
	virtual void SetLooping(bool isLooping) = 0;
	
	virtual bool IsLooping() const = 0;
	
	virtual void SetSpatialization(bool isSpatialized) = 0;

	virtual bool IsSpatialized() const = 0;

	virtual void SetDirectionalAttenuationFactor(float directionalAttenuationFactor) = 0;

	virtual float GetDirectionalAttenuationFactor() const = 0;

	virtual void SetAttenuationModel(ESoundAttenuationModel attenuationModel) = 0;

	virtual ESoundAttenuationModel GetAttenuationModel() const = 0;
#pragma endregion
	
#pragma region EDITABLE_VALUES
	
	virtual float GetMinGain() const = 0;
	
	virtual float GetMaxGain() const = 0;
	
	virtual void SetMinGain(float minGain) = 0;
	
	virtual void SetMaxGain(float maxGain) = 0;
	
	virtual void SetGain(float minGain, float maxGain) = 0;

	virtual void SetGain(Vector2 minMaxGain) = 0;
	
	virtual float GetMinDistance() const = 0;
	
	virtual float GetMaxDistance() const = 0;
	
	virtual void SetMinDistance(float minDistance) = 0;
	
	virtual void SetMaxDistance(float maxDistance) = 0;
	
	virtual void SetDistance(float minDistance, float maxDistance) = 0;

	virtual void SetDistance(Vector2 minMaxDistance) = 0;
	
	virtual Vector3 GetVelocity() const = 0;
	
	virtual void SetVelocity(const Vector3& newVelocity) = 0;

	virtual Vector3 GetDirection() const = 0;

	virtual void SetDirection(const Vector3& newDirection) = 0;

	virtual Vector3 GetPosition() const = 0;

	virtual void SetPosition(const Vector3& newDirection) = 0;
	
	virtual float GetRolloff() const = 0;
	
	virtual void SetRolloff(float newRolloff) = 0;
	
	virtual float GetVolume() const = 0;
	
	virtual void SetVolume(float newVolume) = 0;
	
	virtual float GetDopplerFactor() const = 0;
	
	virtual void SetDopplerFactor(float newDopplerFactor) = 0;
	
	virtual float GetPan() const = 0;
	
	virtual void SetPan(float newPan) = 0;
	
	virtual float GetPitch() const = 0;
	
	virtual void SetPitch(float newPitch) = 0;
#pragma endregion

#pragma region SOUND_ACTIONS
	virtual void Fade(float beginVolume, float endVolume, uint64_t fadeTimeMilli) = 0;
	
	virtual bool IsPlaying() const = 0;
	
	virtual bool IsFinished() const = 0;
	
	virtual void Play() = 0;

	virtual void PlayFromStart() = 0;
	
	virtual void Stop() = 0;

	virtual void PlayWithDelay(uint64_t delayMilli) = 0;
	
	virtual void StopWithDelay(uint64_t delayMilli) = 0;
#pragma endregion
};
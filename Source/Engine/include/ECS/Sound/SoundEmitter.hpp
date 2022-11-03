#pragma once

#include "ECS/Gameobject.hpp"
#include "Types/SoundInterface.hpp"

#include "Generated/SoundEmitter.rfkh.h"

class Sound;
struct ma_sound;

/**
Sound emitter class. Allow to play a sound from a gameObject.
If the object is static, position/direction/velocity are ignored.
*/
class HY_CLASS() ENGINE_API SoundEmitter : public Component, public SoundInterface
{
private:

	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<SoundEmitter> defaultInstantiator(GameObject& GO, const HYGUID& uid);

	//	Variables
private:
#pragma warning(disable:4251)
	std::unique_ptr<ma_sound> m_soundData;
#pragma warning(default:4251)

	bool m_instantiated = false;

	HY_FIELD(POnModify("SetSound"))
		Sound* m_sound = nullptr;

	HY_FIELD(PHeader("State"), POnModify("SetPlayOnStart"))
		bool m_playOnStart = true;					// Should play on scene start
	HY_FIELD(POnModify("SetLooping"))			
		bool m_loop = false;						// Sound looping
	HY_FIELD(POnModify("SetSpatialization"))	
		bool m_spatialized = false;					// Spatialized (3D sound)

	HY_FIELD(PHeader("Parameters"), POnModify("SetVolume"))		
		float m_volume = 1.f;						// Sound volume
	HY_FIELD(POnModify("SetRolloff"))		
		float m_rolloff = 1.f;						// Sound rolling speed (When moving away from a listener)
	HY_FIELD(POnModify("SetDopplerFactor")) 
		float m_dopplerFactor = 1.f;				// Sound doppler factor (Only with spatialization)
	HY_FIELD(POnModify("SetPan"))			
		float m_pan = 0.f;							// Sound panning
	HY_FIELD(POnModify("SetPitch"))			
		float m_pitch = 1.f;						// Sound pitching
	HY_FIELD(POnModify("SetGain"))			
		Vector2 m_gain = Vector2::Zero;				// Gain spatialization min/max


	HY_FIELD(PHeader("Attenuation"), POnModify("SetDirectionalAttenuationFactor"))
		float m_directionalAttenuation = 1.f;															// Attenuation factor based on the sound direction
	HY_FIELD(POnModify("SetDistance"))			
		Vector2 m_attenuationDistance = Vector2(1.f, FLT_MAX);											// Attenuation distances min/max
	HY_FIELD(POnModify("SetAttenuationModel"))		
		ESoundAttenuationModel m_attenuationModel = ESoundAttenuationModel::INVERSE;					// Attenuation model (linear, expo, inverse)
	HY_FIELD(POnModify("SetConeAngles"), PRange(0.f, 360.f))		
		Vector2 m_coneInOutAngles = Vector2(6.28319f * Maths::RADTODEG, 6.28319f * Maths::RADTODEG);	// Sound cone angles for directional attenuation
	HY_FIELD(POnModify("SetConeOuterGain"))		
		float m_coneOuterGain = 0.f;																	// Sound cone outer gain for directional attenuation

public:

	//	Constructors & Destructors

	SoundEmitter(GameObject& gameObject, const HYGUID& uid);
	SoundEmitter(SoundEmitter const& cam) = delete;
	SoundEmitter(SoundEmitter&& cam) = default;

	SoundEmitter& operator=(SoundEmitter const& cam) = delete;
	SoundEmitter& operator=(SoundEmitter && cam) = default;

	~SoundEmitter();

private:
	/**
	Create a copy of the sound data linked to the attached sound resource.
	This allow to play multiple times the same sound.
	*/
	void CreateSoundCopy();

	/**
	Util function for insector modification
	*/
	HY_METHOD()
	void SetSound();

	/**
	Update sound data from SoundEmitter member values
	*/
	void UpdateSoundValues();

public:
	/**
	Start function for the emitter. Call on Game Start / Current scene changes
	*/
	void Start();

#pragma region DE/ACTIVATE PARAMETERS

	/**
	Set the attached resource sound, uninit previous sound and set class data.

	@param sound : Sound resource to attached
	*/
	void SetSound(Sound* sound);

	/**
	Set if the sound need to play when the game is starting.

	@param shouldPlayOnStart : play on start if true, false otherwise
	*/
	HY_METHOD()
	void SetPlayOnStart(bool shouldPlayOnStart);

	/**
	Tell if the sound should be played on game start.

	@return bool : True if playing on start, false otherwise
	*/
	bool ShouldPlayOnStart() const;

	/**
	Pin the emitter to a listener from its index

	@param listenerIndex : Listener index (max 4)
	*/
	void PinListerner(uint32_t listernerIndex);

	/**
	Give the index of the attached listener

	@return uint32_t : Attached listener index
	*/
	uint32_t GetPinnedListenerIndex() const;

	/**
	Set if the sound need to loop or play once

	@param isLooping : Looping if true, play once otherwise
	*/
	HY_METHOD()
	void SetLooping(bool isLooping) override;

	/**
	Tell if the sound is looping

	@return bool : true if the sound is looping, false otherwise
	*/
	bool IsLooping() const override;

	/**
	Set if the sound need to be spatialized (3D sound)

	@param isSpatialized : true if the sound need to be spatialized, false otherwise
	*/
	HY_METHOD()
	void SetSpatialization(bool isSpatialized) override;

	/**
	Tell if the sound is spatialized

	@return bool : true if the sound is spatialized, false otherwise
	*/
	bool IsSpatialized() const override;

	/**
	Set the sound directional attenuation factor (only if spatialized, based on the sound direction)

	@param directionalAttenuationFactor : New attenuation factor
	*/
	HY_METHOD()
	void SetDirectionalAttenuationFactor(float directionalAttenuationFactor) override;

	/**
	Tell the sound current directional attenuation factor

	@return float : Attenuation factor
	*/
	float GetDirectionalAttenuationFactor() const override;

	/**
	Set the sound cone angles for the directional attenuation (only if spatialized)

	@param inOutAnglesInDegrees : New cone angles
	*/
	HY_METHOD()
	void SetConeAngles(Vector2 inOutAnglesInDegrees);

	/**
	Set the sound cone outerGain for the directional attenuation (only if spatialized, refers to cone in/out angles)

	@param outerGain : New cone outer gain
	*/
	HY_METHOD()
	void SetConeOuterGain(float outerGain);

	/**
	Set the sound cone for the directional attenuation (only if spatialized)

	@param innerAngleInDegrees : New cone inner angle
	@param outerAngleInDegrees : New cone outer angle
	@param outerGain : New cone outer gain
	*/
	void SetCone(float innerAngleInDegrees, float outerAngleInDegrees, float outerGain);

	/**
	Give the sound current cone angles

	@return Vector2 : Cone angles
	*/
	Vector2 GetConeAngles();

	/**
	Give the sound current cone outer gain

	@return float : Cone outer gain
	*/
	float GetConeOuterGain();


	/**
	Give the sound current cone parameters

	@param innerAngleInDegrees : Inner angle reference
	@param outerAngleInDegrees : Outer angle reference
	@param outerGain : Outer gain reference
	*/
	void GetCone(float* innerAngleInDegrees, float* outerAngleInDegrees, float* outerGain);

	/**
	Set the sound attenuation model

	@param attenuationModel : New attenuation model
	*/
	void SetAttenuationModel(ESoundAttenuationModel attenuationModel) override;

	/**
	Set the sound attenuation model from int16_t

	@param attenuationModel : New attenuation model
	*/
	HY_METHOD()
	void SetAttenuationModel(int16_t attenuationModel);

	/**
	Give the sound current attenuaton model

	@return ESoundAttenuationModel : Current attenuation model
	*/
	ESoundAttenuationModel GetAttenuationModel() const override;
#pragma endregion

#pragma region EDITABLE_VALUES

	/**
	Give the sound current minimum gain

	@return float : Current minimum gain
	*/
	float GetMinGain() const override;

	/**
	Give the sound current maximum gain

	@return float : Current maximum gain
	*/
	float GetMaxGain() const override;

	/**
	Set the sound minimum gain

	@param minGain : New minimum gain
	*/
	void SetMinGain(float minGain) override;

	/**
	Set the sound maximum gain

	@param maxGain : New maximum gain
	*/
	void SetMaxGain(float maxGain) override;

	/**
	Set the sound gain

	@param minGain : New minimum gain
	@param maxGain : New maximum gain
	*/
	void SetGain(float minGain, float maxGain) override;

	/**
	Set the sound gain

	@param minMaxGain : New gain
	*/
	HY_METHOD()
	void SetGain(Vector2 minMaxGain);

	/**
	Give the sound current minimum distance

	@return float : Current minimum distance
	*/
	float GetMinDistance() const override;

	/**
	Give the sound current maximum distance

	@return float : Current maximum distance
	*/
	float GetMaxDistance() const override;

	/**
	Set the sound minimum distance (for attenuation)

	@param minDistance : New minimum distance
	*/
	void SetMinDistance(float minDistance) override;

	/**
	Set the sound maximum distance (for attenuation)

	@param maxDistance : New maximum distance
	*/
	void SetMaxDistance(float maxDistance) override;

	/**
	Set the sound distances (for attenuation)

	@param minDistance : New minimum distance
	@param maxDistance : New maximum distance
	*/
	void SetDistance(float minDistance, float maxDistance) override;

	/**
	Set the sound distances (for attenuation)

	@param minMaxDistance : New min/max distances
	*/
	HY_METHOD()
	void SetDistance(Vector2 minMaxDistance);

	/**
	Give the sound current velocity

	@return Vector3 : Sound current velocity
	*/
	Vector3 GetVelocity() const override;

	/**
	Set the sound new velocity (combined with doppler factor)

	@param newVelocity : New velocity
	*/
	void SetVelocity(const Vector3& newVelocity) override;

	/**
	Give the sound current direction

	@return Vector3 : Sound current direction
	*/
	Vector3 GetDirection() const override;

	/**
	Set the sound new direction (combined with directional attenuation (cone, directional factor, attenuation model))

	@param newDirection : New direction
	*/
	void SetDirection(const Vector3& newDirection) override;

	/**
	Give the sound current position

	@return Vector3 : Sound current position
	*/
	Vector3 GetPosition() const override;

	/**
	Set the sound new position (combined with spatialization)

	@param newVelocity : New position
	*/
	void SetPosition(const Vector3& newPosition) override;

	/**
	Give the sound current rolloff

	@return float : Sound current rolloff
	*/
	float GetRolloff() const override;

	/**
	Set the sound new rolloff (rolling when moving away from listener)

	@param newRolloff : New sound rolloff
	*/
	HY_METHOD()
	void SetRolloff(float newRolloff) override;

	/**
	Give the sound current volume

	@return float : Sound current volume
	*/
	float GetVolume() const override;

	/**
	Set the sound new volume

	@param newVolume : New sound volume
	*/
	HY_METHOD()
	void SetVolume(float newVolume) override;

	/**
	Give the sound current doppler factor

	@return float : Sound current doppler factor
	*/
	float GetDopplerFactor() const override;

	/**
	Set the sound new doppler factor (combined with sound velocity)

	@param newDopplerFactor : New sound doppler factor
	*/
	HY_METHOD()
	void SetDopplerFactor(float newDopplerFactor) override;

	/**
	Give the sound current pan

	@return float : Sound current pan
	*/
	float GetPan() const override;

	/**
	Set the sound new pan

	@param newPan : New sound pan
	*/
	HY_METHOD()
	void SetPan(float newPan) override;

	/**
	Give the sound current pitch

	@return float : Sound current pan
	*/
	float GetPitch() const override;

	/**
	Set the sound new pitch

	@param newPitch : New sound pitch
	*/
	HY_METHOD()
	void SetPitch(float newPitch) override;
#pragma endregion

#pragma region SOUND_ACTIONS
	/**
	Fade for a given time between to volumes

	@param beginVolume : Volume to start with
	@param endVolume   : Volume to end with
	@param fadeTimeMilli : Fade time in milliseconds
	*/
	void Fade(float beginVolume, float endVolume, uint64_t fadeTimeMilli) override;

	/**
	Tell if the sound is currently playing

	@return bool : True if sound is playing, false otherwise
	*/
	bool IsPlaying() const override;

	/**
	Tell if the sound is finished (cursor at end)

	@return bool : True if sound is finished, false otherwise
	*/
	bool IsFinished() const override;

	/**
	Play the sound if not playing. (Play from cursor current position)
	*/
	void Play() override;

	/**
	Play the sound from the start (Set cursor to position 0).
	The sound is stopped if currently playing.
	*/
	void PlayFromStart() override;

	/**
	Play the sound from a given frame
	*/
	void PlayFrom(uint64_t pcmFrame);

	/**
	Stop the sound if currently playing
	*/
	void Stop() override;

	/**
	Play the sound with a given delay

	@param delayMilli : Time to wait before playing (in milliseconds)
	*/
	void PlayWithDelay(uint64_t delayMilli) override;

	/**
	Stop the sound with a given delay

	@param delayMilli : Time to wait before stoping (in milliseconds)
	*/
	void StopWithDelay(uint64_t delayMilli) override;
#pragma endregion

	SoundEmitter_GENERATED
};

File_SoundEmitter_GENERATED
#pragma once

#include <memory>

#include <Refureku/TypeInfo/Archetypes/Struct.h>

#include "EngineDll.hpp"

struct PimplSystems;

class ParticleSystem;
class BehaviorSystem;
class PhysicsSystem;
class RenderSystem;
class CameraSystem;
class SoundSystem;
class MeshSystem;
class GameObject;
class Component;
class HYGUID;
class Time;

/**
@brief
*/
class SystemManager
{
public:
	// All systems
	std::unique_ptr<PimplSystems> m_pimpl;

public:
	SystemManager();
	~SystemManager();

public:
	void Initialize();
	void Start();
	void Stop();
	void FrameStart();
	void PermanentUpdate();
	void Update(const Time& time);
	void LateUpdate();

	ENGINE_API Component* CreateComponent(rfk::Class const& compClass, GameObject& owner, const HYGUID& id);
	void DestroyComponent(Component& comp);

	static ENGINE_API BehaviorSystem&	GetBehaviorSystem();
	static ENGINE_API ParticleSystem&	GetParticleSystem();
	static ENGINE_API PhysicsSystem&	GetPhysicsSystem();
	static ENGINE_API RenderSystem&		GetRenderSystem();
	static ENGINE_API CameraSystem&		GetCameraSystem();
	static ENGINE_API SoundSystem&		GetSoundSystem();
	static ENGINE_API MeshSystem&		GetMeshSystem();
};
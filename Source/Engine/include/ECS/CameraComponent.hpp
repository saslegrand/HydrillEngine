#pragma once

#include "ECS/Component.hpp"
#include "ECS/Transform.hpp"
#include "Renderer/RenderObjects/Camera.hpp"

#include "Generated/CameraComponent.rfkh.h"

class GameObject;

/**
@brief Camera component, handle a camera to render image in game (linked to the CameraSystem)
*/
class HY_CLASS() ENGINE_API CameraComponent : public Component
{
private:

	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<CameraComponent> defaultInstantiator(GameObject& GO, const HYGUID& uid);

//	Variables

public :

	HY_FIELD() Camera camera;

	Transform& transform;

//	Constructors & Destructors

public:
	CameraComponent(GameObject& gameObject, const HYGUID& uid);
	CameraComponent(CameraComponent const& cam) = delete;
	CameraComponent(CameraComponent&& cam) = default;

	CameraComponent& operator=(CameraComponent const& cam) = delete;
	CameraComponent& operator=(CameraComponent&& cam) = default;

	~CameraComponent();

	/**
	@brief Update self camera transform from owner transform
	*/
	void UpdateCamera();

	void OnModify() override;

	CameraComponent_GENERATED
};

File_CameraComponent_GENERATED
#pragma once

#include "ECS/Gameobject.hpp"

#include "Generated/CanvasComponent.rfkh.h"

/**
@brief Camera component, handle a camera to render image in game (linked to the CameraSystem)
*/
class HY_CLASS() ENGINE_API CanvasComponent : public Component
{
private:

	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<CanvasComponent> defaultInstantiator(GameObject& GO, const HYGUID& uid);

//	Variables

public :

	Transform& transform;

//	Constructors & Destructors

public:
	CanvasComponent(GameObject& gameObject, const HYGUID& uid);
	CanvasComponent(CanvasComponent const& cam) = delete;
	CanvasComponent(CanvasComponent&& cam) = default;

	CanvasComponent& operator=(CanvasComponent const& cam) = delete;
	CanvasComponent& operator=(CanvasComponent&& cam) = default;

	~CanvasComponent();

	CanvasComponent_GENERATED
};

File_CanvasComponent_GENERATED
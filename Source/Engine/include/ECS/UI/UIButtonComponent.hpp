#pragma once

#include "ECS/Gameobject.hpp"

#include "Generated/UIButtonComponent.rfkh.h"

/**
@brief Camera component, handle a camera to render image in game (linked to the CameraSystem)
*/
class HY_CLASS() ENGINE_API UIButtonComponent : public Component
{
private:

	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<UIButtonComponent> defaultInstantiator(GameObject& GO, const HYGUID& uid);

//	Variables

public :
	Transform& transform;

//	Constructors & Destructors

public:
	UIButtonComponent(GameObject& gameObject, const HYGUID& uid);
	UIButtonComponent(UIButtonComponent const& cam) = delete;
	UIButtonComponent(UIButtonComponent&& cam) = default;

	UIButtonComponent& operator=(UIButtonComponent const& cam) = delete;
	UIButtonComponent& operator=(UIButtonComponent&& cam) = default;

	~UIButtonComponent();

	UIButtonComponent_GENERATED
};

File_UIButtonComponent_GENERATED
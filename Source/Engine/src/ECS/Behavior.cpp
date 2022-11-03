#include "EngineContext.hpp"

#include "Generated/Behavior.rfks.h"

Behavior::Behavior(GameObject& gameObject, const HyString& name, const HYGUID& uid)
	: Component(gameObject, name, uid)
{
}

Behavior::~Behavior()
{
}

void Behavior::SetActive(bool value)
{
	// Check if value is different from the current object state
	if (value == IsActive())
		return;

	// Change the current object state
	SceneObject::SetActive(value);

	// Call OnEnable/Disable from value
	value == true ? OnEnable() : OnDisable();
}
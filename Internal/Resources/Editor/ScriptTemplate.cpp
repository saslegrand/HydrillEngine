
// Add other include here
// ----------------------


// Script generated source file
// This MUST be the last include, other include symbols are needed
// ---------------------------------------------------------------
#include "Generated/$$SCRIPT_NAME$$.rfks.h"

rfk::UniquePtr<$$SCRIPT_NAME$$> $$SCRIPT_NAME$$::defaultInstantiatorID(GameObject& gameObject, const HYGUID& uid)
{
	return rfk::makeUnique<$$SCRIPT_NAME$$>(gameObject, uid);
}

$$SCRIPT_NAME$$::$$SCRIPT_NAME$$(GameObject& gameObject, const HYGUID& uid)
	: Behavior(gameObject, getArchetype().getName(), uid)
{
	// Allow multiple $$SCRIPT_NAME$$ instances in the same GameObject
	// Switch to have unique instance
	m_isUnique = false;
}

void $$SCRIPT_NAME$$::Start()
{
	// Write your code
}

void $$SCRIPT_NAME$$::Update(float tick)
{
	// Write your code
}
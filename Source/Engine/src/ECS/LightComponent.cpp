#include "Renderer/RenderSystem.hpp"
#include "EngineContext.hpp"

#include "Generated/LightComponent.rfks.h"


rfk::UniquePtr<LightComponent> LightComponent::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<LightComponent>(GO, uid);
}



LightComponent::LightComponent(GameObject& gameObject, const HYGUID& uid)
	: Component(gameObject, getArchetype().getName(), uid)
{
	m_isUnique = true;
}


LightComponent::~LightComponent()
{
}

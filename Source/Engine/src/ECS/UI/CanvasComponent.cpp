//#include "EngineContext.hpp"

#include "Generated/CanvasComponent.rfks.h"


rfk::UniquePtr<CanvasComponent> CanvasComponent::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<CanvasComponent>(GO, uid);
}


CanvasComponent::CanvasComponent(GameObject& gameObject, const HYGUID& uid)
	: Component(gameObject, getArchetype().getName(), uid), transform(gameObject.transform)
{
	m_isUnique = true;

	//EngineContext::Instance().cameraSystem->Register(*this);
}

CanvasComponent::~CanvasComponent()
{
	//EngineContext::Instance().cameraSystem->Unregister(*this);
}

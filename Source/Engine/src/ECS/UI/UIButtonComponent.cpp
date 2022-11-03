//#include "EngineContext.hpp"

#include "Generated/UIButtonComponent.rfks.h"


rfk::UniquePtr<UIButtonComponent> UIButtonComponent::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<UIButtonComponent>(GO, uid);
}

UIButtonComponent::UIButtonComponent(GameObject& gameObject, const HYGUID& uid)
	: Component(gameObject, getArchetype().getName(), uid), transform(gameObject.transform)
{
	m_isUnique = true;

	//EngineContext::Instance().cameraSystem->Register(*this);
}

UIButtonComponent::~UIButtonComponent()
{
	//EngineContext::Instance().cameraSystem->Unregister(*this);
}

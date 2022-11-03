#include "EngineContext.hpp"

#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/GameObject.hpp"

#include "Generated/CameraComponent.rfks.h"


rfk::UniquePtr<CameraComponent> CameraComponent::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<CameraComponent>(GO, uid);
}


CameraComponent::CameraComponent(GameObject& gameObject, const HYGUID& uid)
	: Component(gameObject, getArchetype().getName(), uid), transform(gameObject.transform)
{
	m_isUnique = true;
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::UpdateCamera()
{
	camera.SetTransform(transform.Position(), transform.Rotation());
	camera.Update();
}

void CameraComponent::OnModify()
{
}

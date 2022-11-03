#include "ECS/GameObject.hpp"

#include "Generated/Component.rfks.h"

Component::Component(GameObject& gameObject, const HyString& name, const HYGUID& uid)
	: SceneObject(name, uid), gameObject(gameObject)
{

}

Component::~Component()
{
}

bool Component::IsUnique() const
{
	return m_isUnique;
}

void Component::OnModify()
{

}

void Component::OnDestroy()
{

}

void Component::SetAsDestroyed()
{
	m_isDestroyed = true;
}

bool Component::IsDestroyed() const
{
	return m_isDestroyed;
}

void Component::SetActive(bool isActive)
{
	m_isActive = isActive;
}

bool Component::IsActive() const
{
	return m_isActive && gameObject.IsActive();
}
#include "Generated/SceneObject.rfks.h"

SceneObject::SceneObject(const HyString& name, const HYGUID& uid)
	: m_name(name), m_UID(uid)
{

}

void SceneObject::SetActive(bool value)
{
	m_isActive = value;
}

bool SceneObject::IsSelfActive() const
{
	return m_isActive;
}
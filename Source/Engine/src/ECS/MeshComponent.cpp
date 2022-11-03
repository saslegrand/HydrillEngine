#include "ECS/Systems/MeshSystem.hpp"
#include "EngineContext.hpp"
#include "Resources/ResourcesManager.hpp"
#include "Tools/PathConfig.hpp"

#include "Generated/MeshComponent.rfks.h"


rfk::UniquePtr<MeshComponent> MeshComponent::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<MeshComponent>(GO, uid);
}


MeshComponent::MeshComponent(GameObject& gameObject, const HYGUID& uid)
	: Component(gameObject, getArchetype().getName(), uid)
{
	m_isUnique = true;
}


MeshComponent::~MeshComponent()
{
}

void MeshComponent::Initialize()
{
	SystemManager::GetMeshSystem().UpdateMeshComponent(*this);
}

void MeshComponent::SetActive(bool value)
{
	Component::SetActive(value);
	SystemManager::GetMeshSystem().UpdateMeshComponent(*this);
}

void MeshComponent::SetMesh(Mesh* newMesh)
{
	m_mesh = newMesh;
	SystemManager::GetMeshSystem().UpdateMeshComponent(*this);
}

Mesh* MeshComponent::GetMesh() const
{
	return m_mesh;
}

void MeshComponent::OnModify()
{
	SystemManager::GetMeshSystem().UpdateMeshComponent(*this);
}

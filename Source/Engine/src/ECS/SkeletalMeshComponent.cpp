#include "Renderer/RenderSystem.hpp"
#include "ECS/Systems/MeshSystem.hpp"
#include "EngineContext.hpp"

#include "Generated/SkeletalMeshComponent.rfks.h"



rfk::UniquePtr<SkeletalMeshComponent> SkeletalMeshComponent::defaultInstantiator(GameObject& GO, const HYGUID& uid)
{
	return rfk::makeUnique<SkeletalMeshComponent>(GO, uid);
}

SkeletalMeshComponent::SkeletalMeshComponent(GameObject& gameObject, const HYGUID& uid)
	: Component(gameObject, getArchetype().getName(), uid)
{
	m_isUnique = true;
	//EngineContext::Instance().meshSystem->Register(*this);
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
	//EngineContext::Instance().meshSystem->Unregister(*this);
}

void SkeletalMeshComponent::Initialize()
{
	SystemManager::GetMeshSystem().UpdateSkeletalMeshComponent(*this);
}

void SkeletalMeshComponent::AddLinkedBoneMatrix(GameObject* currentNode, SkeletalData& skeletalData, int number)
{
	if (number <= 0)
		return;

	const std::string& name = currentNode->GetName();

	if (skeletalData.boneLink.find(name) != skeletalData.boneLink.end())
	{
		int id = skeletalData.boneLink[name].id;
		skeletalData.boneTransforms[id] = globalInverseTransform * currentNode->transform.GetWorldMatrix() * skeletalData.boneLink[name].offsetMatrix;
		number--;
	}

	for (GameObject* child : currentNode->GetChildren())
	{
		AddLinkedBoneMatrix(child, skeletalData, number);
	}
}

void SkeletalMeshComponent::ComputeBoneMatrices()
{
	if (!m_mesh || !skinRoot || !skinRoot->GetParent())
		return;

	globalInverseTransform = Matrix4::Inverse(gameObject.transform.GetWorldMatrix());

	for (int i = 0; i < m_mesh->skeletonDatas.size(); i++)
	{
		AddLinkedBoneMatrix(skinRoot, m_mesh->skeletonDatas[i],static_cast<int>(m_mesh->skeletonDatas[i].boneLink.size()));
	}
}

void SkeletalMeshComponent::SetMesh(SkeletalMesh* newMesh)
{
	m_mesh = newMesh;
}

void SkeletalMeshComponent::SetActive(bool value)
{
	Component::SetActive(value);
	SystemManager::GetMeshSystem().UpdateSkeletalMeshComponent(*this);
}

SkeletalMesh* SkeletalMeshComponent::GetMesh() const
{
	return m_mesh;
}

void SkeletalMeshComponent::OnModify()
{
	SystemManager::GetMeshSystem().UpdateSkeletalMeshComponent(*this);
}
#include "ECS/Systems/MeshSystem.hpp"

#include "ECS/MeshComponent.hpp"
#include "ECS/SkeletalMeshComponent.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Core/Logger.hpp"
#include "EngineContext.hpp"


template<typename TPairComponentInstances>
void MeshSystem::SendInstancesToRenderSystem(TPairComponentInstances& componentInstances)
{
	if (componentInstances.second.currentMesh == nullptr)
	{
		if (GenerateSubMeshesInstances(*componentInstances.first))
		{
			SendToRenderSystem(componentInstances.second);
		}
		return;
	}
	if (!componentInstances.second.registered)
	{
		SendToRenderSystem(componentInstances.second);
	}
}


void MeshSystem::UpdateMeshesWithMaterial(Material* material)
{
	RenderSystem& renderSystem = SystemManager::GetRenderSystem();
	for (MeshInstance* instance : m_meshInstancesFromMat[material])
	{
		renderSystem.Unregister(instance);
		renderSystem.Register(instance);
	}

	for (SkeletalMeshInstance* instance : m_skeletalMeshInstancesFromMat[material])
	{
		renderSystem.Unregister(instance);
		renderSystem.Register(instance);
	}
}

void MeshSystem::Update()
{
	for (auto& element : m_skMeshCompInstances)
	{
		element.first->ComputeBoneMatrices();
	}
}



//	MESHES	FUNCTIONS
//	-----------------

bool MeshSystem::GenerateSubMeshesInstances(MeshComponent& meshComp)
{
	Mesh* mesh = meshComp.GetMesh();

	m_meshCompInstances[&meshComp] = SubmeshesInstances();

	if (mesh == nullptr) return false;

	SubmeshesInstances& submeshesInstances = m_meshCompInstances[&meshComp];

	submeshesInstances.currentMesh = mesh;
	auto& instances = submeshesInstances.instances;

	int materialNb = static_cast<int>(meshComp.materials.size());
	for (int i = 0; i < mesh->subMeshes.size(); ++i)
	{
		Material* material = (materialNb > 0 && meshComp.materials[i % materialNb]) ?
			meshComp.materials[i % materialNb] : nullptr;

		MeshInstance& instance = instances.emplace_back(&mesh->subMeshes[i], material, &meshComp.gameObject.transform);
		instance.isActive = meshComp.IsActive();

		if (material) m_meshInstancesFromMat[material].emplace_back(&instance);
	}

	for (const Material* mat : meshComp.materials)
	{
		if (mat == nullptr) continue;
		submeshesInstances.materialsPipelines.emplace_back(mat->shader, mat->surface);
	}

	return true;
}


void MeshSystem::DegenerateSubMeshesInstances(MeshComponent& meshComp)
{
	auto it = m_meshCompInstances.find(&meshComp);
	if (it != m_meshCompInstances.end())
	{
		RemoveFromRenderSystem(m_meshCompInstances[&meshComp]);

		//	Remove instances from "m_meshInstancesFromMat"
		for (MeshInstance& instance : m_meshCompInstances[&meshComp].instances)
		{
			if (instance.material == nullptr) continue;

			auto& boundInstance = m_meshInstancesFromMat[instance.material];

			auto it = std::find(boundInstance.begin(), boundInstance.end(), &instance);

			if (it != boundInstance.end())
			{
				boundInstance.erase(it);
			}
		}

		m_meshCompInstances.erase(it);
	}
}


void MeshSystem::UpdateMeshComponent(MeshComponent& meshComp)
{
	//	If it doesn't exist yet, generate instances and send them to the renderSystem
	if (m_meshCompInstances.find(&meshComp) == m_meshCompInstances.end())
	{
		if (GenerateSubMeshesInstances(meshComp))
		{
			SendToRenderSystem(m_meshCompInstances[&meshComp]);
		}

		return;
	}

	//	Else degenerate instances and regenerate them, then send them to the render system

	DegenerateSubMeshesInstances(meshComp);

	if (GenerateSubMeshesInstances(meshComp))
	{
		SendToRenderSystem(m_meshCompInstances[&meshComp]);
	}
}


void MeshSystem::SendToRenderSystem(SubmeshesInstances& meshComp)
{
	for (MeshInstance& instance : meshComp.instances)
	{
		SystemManager::GetRenderSystem().Register(&instance);
	}
	meshComp.registered = true;
}


void MeshSystem::RemoveFromRenderSystem(SubmeshesInstances& meshComp)
{
	for (MeshInstance& instance : meshComp.instances)
	{
		SystemManager::GetRenderSystem().Unregister(&instance);
	}
	meshComp.registered = false;
}


MeshComponent* MeshSystem::AddMeshInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id)
{
	MeshComponent& meshComp = *m_meshComponents.emplace_back(archetype.makeUniqueInstance<MeshComponent>(owner, id));

	if (GenerateSubMeshesInstances(meshComp))
		SendToRenderSystem(m_meshCompInstances[&meshComp]);

	return &meshComp;
}

void MeshSystem::RemoveMeshInstance(MeshComponent& meshComp)
{
	auto it = std::find_if(m_meshComponents.begin(), m_meshComponents.end(),
		[&meshComp](std::unique_ptr<MeshComponent>& mesh) { return mesh.get() == &meshComp; });

	if (it == m_meshComponents.end())
	{
		Logger::Error("MeshSystem - The mesh component you tried to remove not exists");
		return;
	}

	// Erase skeletal component
	DegenerateSubMeshesInstances(meshComp);
	m_meshComponents.erase(it);
}

//	SKELETAL MESHES	FUNCTIONS
//	-------------------------

bool MeshSystem::GenerateSubMeshesInstances(SkeletalMeshComponent& skMeshComp)
{
	SkeletalMesh* skmesh = skMeshComp.GetMesh();

	m_skMeshCompInstances[&skMeshComp] = SubSkeletalMeshesInstances();

	if (skmesh == nullptr) return false;

	SubSkeletalMeshesInstances& subSkeletalMeshesInstances = m_skMeshCompInstances[&skMeshComp];

	subSkeletalMeshesInstances.currentMesh = skmesh;
	auto& instances = subSkeletalMeshesInstances.instances;

	int materialNb = static_cast<int>(skMeshComp.materials.size());
	for (int i = 0; i < skmesh->subMeshes.size(); ++i)
	{
		Material* material = (materialNb > 0 && skMeshComp.materials[i % materialNb]) ?
			skMeshComp.materials[i % materialNb] : nullptr;

		SkeletalData* skeleton = i < skmesh->skeletonDatas.size() ? &skmesh->skeletonDatas[i] : nullptr;

		SkeletalMeshInstance& instance = instances.emplace_back(&skmesh->subMeshes[i], material, &skMeshComp.gameObject.transform, skeleton);
		instance.isActive = skMeshComp.IsActive();

		if (material) m_skeletalMeshInstancesFromMat[material].emplace_back(&instance);
	}

	for (const Material* mat : skMeshComp.materials)
	{
		if (mat == nullptr) continue;
		subSkeletalMeshesInstances.materialsPipelines.emplace_back(mat->shader, mat->surface);
	}

	return true;
}


void MeshSystem::DegenerateSubMeshesInstances(SkeletalMeshComponent& skMeshComp)
{
	auto it = m_skMeshCompInstances.find(&skMeshComp);
	if (it != m_skMeshCompInstances.end())
	{
		RemoveFromRenderSystem(m_skMeshCompInstances[&skMeshComp]);

		//	Remove instances from "m_meshInstancesFromMat"
		for (MeshInstance& instance : m_skMeshCompInstances[&skMeshComp].instances)
		{
			if (instance.material == nullptr) continue;

			auto& boundInstance = m_skeletalMeshInstancesFromMat[instance.material];

			auto it = std::find(boundInstance.begin(), boundInstance.end(), &instance);

			if (it != boundInstance.end())
			{
				boundInstance.erase(it);
			}
		}

		m_skMeshCompInstances.erase(it);
	}
}


void MeshSystem::UpdateSkeletalMeshComponent(SkeletalMeshComponent& skMeshComp)
{
	//	If it doesn't exist yet, generate instances and send them to the renderSystem
	if (m_skMeshCompInstances.find(&skMeshComp) == m_skMeshCompInstances.end())
	{
		if (GenerateSubMeshesInstances(skMeshComp))
		{
			SendToRenderSystem(m_skMeshCompInstances[&skMeshComp]);
		}

		return;
	}

	//	Else degenerate instances and regenerate them, then send them to the render system

	DegenerateSubMeshesInstances(skMeshComp);

	if (GenerateSubMeshesInstances(skMeshComp))
	{
		SendToRenderSystem(m_skMeshCompInstances[&skMeshComp]);
	}
}


void MeshSystem::SendToRenderSystem(SubSkeletalMeshesInstances& subSkeletalMeshinstances)
{
	for (SkeletalMeshInstance& instance : subSkeletalMeshinstances.instances)
	{
		SystemManager::GetRenderSystem().Register(&instance);
	}
	subSkeletalMeshinstances.registered = true;
}


void MeshSystem::RemoveFromRenderSystem(SubSkeletalMeshesInstances& subSkeletalMeshinstances)
{
	for (SkeletalMeshInstance& instance : subSkeletalMeshinstances.instances)
	{
		SystemManager::GetRenderSystem().Unregister(&instance);
	}
	subSkeletalMeshinstances.registered = false;
}


SkeletalMeshComponent* MeshSystem::AddSkMeshInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id)
{
	SkeletalMeshComponent& meshComp = *m_skeletakMeshComponents.emplace_back(archetype.makeUniqueInstance<SkeletalMeshComponent>(owner, id));

	if (GenerateSubMeshesInstances(meshComp))
		SendToRenderSystem(m_skMeshCompInstances[&meshComp]);

	return &meshComp;
}

void MeshSystem::RemoveSkMeshInstance(SkeletalMeshComponent& skMeshComp)
{
	auto it = std::find_if(m_skeletakMeshComponents.begin(), m_skeletakMeshComponents.end(), 
		[&skMeshComp](std::unique_ptr<SkeletalMeshComponent>& skMesh) { return skMesh.get() == &skMeshComp; });

	if (it == m_skeletakMeshComponents.end())
	{
		Logger::Error("MeshSystem - The skmesh component you tried to remove not exists");
		return;
	}

	// Erase skeletal component
	DegenerateSubMeshesInstances(skMeshComp);
	m_skeletakMeshComponents.erase(it);
}
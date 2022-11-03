
#include <fstream>

#include "Resources/Resource/Material.hpp"
#include "ECS/Systems/SystemManager.hpp"

#include "EngineContext.hpp"
#include "Core/Logger.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Resources/ResourcesManager.hpp"

#include "Generated/SkeletalMesh.rfks.h"

rfk::UniquePtr<SkeletalMesh> SkeletalMesh::defaultInstantiator(const HYGUID& uid)
{
	return rfk::makeUnique<SkeletalMesh>(uid);
}

SkeletalMesh::SkeletalMesh(const HYGUID& uid)
	: Mesh(uid, staticGetArchetype().getName())
{
	m_type = RESOURCE_TYPE::SKELETALMESH;
}

void SkeletalMesh::LoadInGPUMemory()
{
	SystemManager::GetRenderSystem().GPUWrapper.CreateSkeletalMeshData(*this);
}

void SkeletalMesh::UnloadFromGPUMemory()
{
	SystemManager::GetRenderSystem().GPUWrapper.CreateSkeletalMeshData(*this);
}

void SkeletalMesh::Serialize()
{
	std::ofstream o(m_filepath, std::ifstream::binary);
	if (!o)
	{
		Logger::Warning("Cannot open file " + m_filepath);
		return;
	}

	HYGUID defaultGUID;

	o.write(reinterpret_cast<const char*>(&m_boundingBox.min), sizeof(Vector3));
	o.write(reinterpret_cast<const char*>(&m_boundingBox.max), sizeof(Vector3));

	int subMeshesNb = static_cast<int>(subMeshes.size());
	o.write(reinterpret_cast<const char*>(&subMeshesNb), sizeof(int));
	for (int i = 0; i < subMeshesNb; ++i)
	{
		const MeshData& data = subMeshes[i];
		const SkeletalData& skData = skeletonDatas[i];

		int verticesCount = static_cast<int>(data.vertices.size());
		int indicesCount = static_cast<int>(data.indices.size());
		int bonesCount = static_cast<int>(skData.boneLink.size());

		o.write(reinterpret_cast<const char*>(data.material ? &data.material->GetUID() : &defaultGUID), sizeof(HYGUID));
		o.write(reinterpret_cast<const char*>(&verticesCount), sizeof(int));
		o.write(reinterpret_cast<const char*>(&indicesCount), sizeof(int));
		o.write(reinterpret_cast<const char*>(&bonesCount), sizeof(int));
		o.write(reinterpret_cast<const char*>(data.vertices.data()), data.vertices.size() * sizeof(Vertex));
		o.write(reinterpret_cast<const char*>(data.indices.data()), data.indices.size() * sizeof(int));
		o.write(reinterpret_cast<const char*>(skData.vertexBoneData.data()), skData.vertexBoneData.size() * sizeof(VertexBoneData));
		for (auto& it : skData.boneLink)
		{
			int size = static_cast<int>(it.first.size());
			o.write(reinterpret_cast<const char*>(&size), sizeof(int));
			o.write(reinterpret_cast<const char*>(it.first.c_str()), size * sizeof(char));
			o.write(reinterpret_cast<const char*>(&it.second.id), sizeof(int));
			o.write(reinterpret_cast<const char*>(&it.second.offsetMatrix), sizeof(Matrix4));
		}
	}

	o.close();
}

void SkeletalMesh::Deserialize()
{
	std::ifstream i(m_filepath, std::ifstream::binary);
	if (!i)
	{
		Logger::Warning("Can't load asset file : " + m_filepath);
		return;
	}

	subMeshes.clear();
	skeletonDatas.clear();

	i.read(reinterpret_cast<char*>(&m_boundingBox.min), sizeof(Vector3));
	i.read(reinterpret_cast<char*>(&m_boundingBox.max), sizeof(Vector3));

	int subMeshesNb = 0;
	i.read(reinterpret_cast<char*>(&subMeshesNb), sizeof(int));
	for (int j = 0; j < subMeshesNb; ++j)
	{
		MeshData data;
		int verticesCount = 0;
		int indicesCount = 0;

		SkeletalData skData;
		int bonesCount = 0;

		HYGUID matId;
		i.read(reinterpret_cast<char*>(&matId), sizeof(HYGUID));
		data.material = EngineContext::Instance().resourcesManager->GetResource<Material>(matId);

		i.read(reinterpret_cast<char*>(&verticesCount), sizeof(int));
		i.read(reinterpret_cast<char*>(&indicesCount), sizeof(int));
		i.read(reinterpret_cast<char*>(&bonesCount), sizeof(int));

		data.vertices.resize(verticesCount);
		data.indices.resize(indicesCount);
		skData.vertexBoneData.resize(verticesCount);
		skData.boneLink.reserve(bonesCount);

		i.read(reinterpret_cast<char*>(data.vertices.data()), verticesCount * sizeof(Vertex));
		i.read(reinterpret_cast<char*>(data.indices.data()), indicesCount * sizeof(int));
		i.read(reinterpret_cast<char*>(skData.vertexBoneData.data()), verticesCount * sizeof(VertexBoneData));

		for (int k = 0; k < bonesCount; ++k)
		{
			int nameSize = 0;
			std::string name;
			int id = -1;
			Matrix4 mat = Matrix4::Identity;
			i.read(reinterpret_cast<char*>(&nameSize), sizeof(int));
			name.resize(nameSize);
			i.read(reinterpret_cast<char*>(&name[0]), nameSize);
			i.read(reinterpret_cast<char*>(&id), sizeof(int));
			i.read(reinterpret_cast<char*>(&mat), sizeof(Matrix4));
			skData.boneLink[name].id = id;
			skData.boneLink[name].offsetMatrix = mat;
			skData.boneTransforms.push_back(Matrix4::Identity);
		}

		subMeshes.push_back(data);
		skeletonDatas.push_back(skData);
	}

	i.close();
}
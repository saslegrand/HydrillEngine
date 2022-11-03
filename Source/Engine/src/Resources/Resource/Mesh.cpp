#include <fstream>

#include "Resources/Resource/Material.hpp"
#include "ECS/Systems/SystemManager.hpp"

#include "EngineContext.hpp"
#include "Core/Logger.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Resources/ResourcesManager.hpp"

#include "Generated/Mesh.rfks.h"

rfk::UniquePtr<Mesh> Mesh::defaultInstantiator(const HYGUID& uid)
{
	return rfk::makeUnique<Mesh>(uid);
}

Mesh::Mesh(const HYGUID& uid, const std::string& name)
	: Resource(uid, name)
{
	m_type = RESOURCE_TYPE::MESH;
}


Mesh::Mesh(const HYGUID& uid)
	: Resource(uid, staticGetArchetype().getName())
{
	m_type = RESOURCE_TYPE::MESH;
}

void Mesh::LoadInGPUMemory()
{
	SystemManager::GetRenderSystem().GPUWrapper.CreateMeshData(*this);
}

void Mesh::UnloadFromGPUMemory()
{
	SystemManager::GetRenderSystem().GPUWrapper.RemoveMeshData(*this);
}

void Mesh::Serialize()
{
	int subMeshesNb = static_cast<int>(subMeshes.size());
	// If we erased vertices after creating data, we can't save again anymore
	if (subMeshesNb <= 0 || subMeshes[0].vertices.size() <= 0)
		return;

	std::ofstream o(m_filepath, std::ifstream::binary);
	if (!o)
	{
		Logger::Warning("Cannot open file " + m_filepath);
		return;
	}
	o.write(reinterpret_cast<const char*>(&m_boundingBox.min), sizeof(Vector3));
	o.write(reinterpret_cast<const char*>(&m_boundingBox.max), sizeof(Vector3));

	o.write(reinterpret_cast<const char*>(&subMeshesNb), sizeof(int));
	for (int i = 0; i < subMeshesNb; ++i)
	{
		const MeshData& data = subMeshes[i];
		int verticesCount = static_cast<int>(data.vertices.size());
		int indicesCount = static_cast<int>(data.indices.size());

		HYGUID defaultGUID;
		o.write(reinterpret_cast<const char*>(data.material ? &data.material->GetUID() : &defaultGUID), sizeof(HYGUID));
		o.write(reinterpret_cast<const char*>(&verticesCount), sizeof(int));
		o.write(reinterpret_cast<const char*>(&indicesCount), sizeof(int));
		o.write(reinterpret_cast<const char*>(data.vertices.data()), data.vertices.size() * sizeof(Vertex));
		o.write(reinterpret_cast<const char*>(data.indices.data()), data.indices.size() * sizeof(int));
	}

	o.close();
}

void Mesh::Deserialize()
{
	std::ifstream i(m_filepath, std::ifstream::binary);
	if (!i)
	{
		Logger::Warning("Can't load asset file : " + m_filepath);
		return;
	}

	subMeshes.clear();

	i.read(reinterpret_cast<char*>(&m_boundingBox.min), sizeof(Vector3));
	i.read(reinterpret_cast<char*>(&m_boundingBox.max), sizeof(Vector3));

	int subMeshesNb = 0;
	i.read(reinterpret_cast<char*>(&subMeshesNb), sizeof(int));
	for (int k = 0; k < subMeshesNb; ++k)
	{
		subMeshes.push_back(MeshData());
		MeshData& data = subMeshes.back();

		int verticesCount = 0;
		int indicesCount = 0;

		HYGUID matId;
		i.read(reinterpret_cast<char*>(&matId), sizeof(HYGUID));
		data.material = EngineContext::Instance().resourcesManager->GetResource<Material>(matId);

		i.read(reinterpret_cast<char*>(&verticesCount), sizeof(int));
		i.read(reinterpret_cast<char*>(&indicesCount), sizeof(int));

		data.vertices.resize(verticesCount);
		data.indices.resize(indicesCount);
		i.read(reinterpret_cast<char*>(data.vertices.data()), verticesCount * sizeof(Vertex));
		i.read(reinterpret_cast<char*>(data.indices.data()), indicesCount * sizeof(int));
	}

	i.close();
}


const BoundingBox& Mesh::GetBoudingBox() const
{
	return m_boundingBox;
}

void Mesh::SetBoudingBox(const Vector3& min, const Vector3& max)
{
	m_boundingBox.min = min;
	m_boundingBox.max = max;
}
#include <iostream>
//#include <stdio.h>
#include <fstream>
#include <nlohmann/json.hpp>

#include "Resources/Loaders/ResourcesLoader.hpp"
#include "EngineContext.hpp"
#include "Renderer/RenderSystem.hpp"

#include "Tools/StringHelper.hpp"
#include "Tools/SerializationUtils.hpp"

#include "EngineContext.hpp"
#include "Resources/ResourcesManager.hpp"

#include "Generated/Model.rfks.h"

rfk::UniquePtr<Model> Model::defaultInstantiator(const HYGUID& uid)
{
	return rfk::makeUnique<Model>(uid);
}

Model::Model(const HYGUID& uid)
	: Prefab(uid, staticGetArchetype().getName())
{
	m_type = RESOURCE_TYPE::MODEL;
}

const std::vector<Mesh*>& Model::GetMeshes() const
{
	return m_meshes;
}

const std::vector<TransformData>& Model::GetMeshesOffsets() const
{
	return m_meshesOffsets;
}

const std::vector<Material*>& Model::GetMaterials() const
{
	return m_materials;
}

void Model::AddMesh(Mesh* mesh, const TransformData& offset)
{
	m_meshes.emplace_back(mesh);
	
	m_meshesOffsets.emplace_back(offset);
}


void Model::AddMaterial(Material& material)
{
	m_materials.emplace_back(&material);
}

// Call when a new resource is added to the project
bool Model::Import(const std::string& path)
{
	m_originalPath = path;
	Resource::Import(StringHelper::GetFileNameWithoutExtension(path) + ".model");

	return ResourcesLoader::LoadModel(*this);
}

void Model::Serialize()
{
	json j;

	j["extension"] = StringHelper::GetFileExtensionFromPath(m_originalPath);

	Serialization::SetContainer<float>(j["boudingBoxMin"], m_boundingBox.min);
	Serialization::SetContainer<float>(j["boudingBoxMax"], m_boundingBox.max);

	for (auto& mesh : m_meshes)
		j["meshes"].push_back(std::string(mesh->GetUID()));

	json& offsets = j["offsets"];

	int index = 0;
	for (auto transformOffset : m_meshesOffsets)
	{
		json currentOffset;

		Serialization::SetContainer<float>(currentOffset["position"], transformOffset.position);
		Serialization::SetContainer<float>(currentOffset["scale"], transformOffset.scale);
		Serialization::SetContainer<float>(currentOffset["rotation"], transformOffset.rotation);
		offsets.push_back(currentOffset);

		index++;
	}

	for (auto material : m_materials)
		j["materials"].push_back(std::string(material->GetUID()));

	j["prefab"] = m_jsonGraph;

	WriteJson(m_filepath, j);
}

void Model::Deserialize()
{
	json j;
	ReadJson(m_filepath, j);

	ResourcesManager* RM = EngineContext::Instance().resourcesManager;

	std::string ext = "";
	Serialization::TryGetValue(j, "extension", ext);
	m_originalPath = StringHelper::GetFilePathWithoutExtension(m_filepath) + ext;

	Serialization::TryGetContainer<float>(j, "boudingBoxMin", m_boundingBox.min);
	Serialization::TryGetContainer<float>(j, "boudingBoxMax", m_boundingBox.max);

	for (const auto& it : j["meshes"].items())
	{
		Mesh* m = RM->GetResource<Mesh>(HYGUID(it.value().get<std::string>()));
		if (m != nullptr)
			m_meshes.push_back(m);
	}
	
	for (const auto& it : j["offsets"].items())
	{
		json& current = it.value();

		TransformData& transformOffset = m_meshesOffsets.emplace_back();

		Serialization::TryGetContainer<float>(current, "position", transformOffset.position);
		Serialization::TryGetContainer<float>(current, "scale", transformOffset.scale);
		Serialization::TryGetContainer<float>(current, "rotation", transformOffset.rotation);

		transformOffset.ComputeLocalTRS();
	}

	for (const auto& it : j["materials"].items())
	{
		Material* m = RM->GetResource<Material>(HYGUID(it.value().get<std::string>()));
		if (m != nullptr)
			m_materials.push_back(m);
	}
	
	m_jsonGraph = j["prefab"];
}

const BoundingBox& Model::GetBoudingBox() const
{
	return m_boundingBox;
}

void Model::SetBoudingBox(const Vector3& min, const Vector3& max)
{
	m_boundingBox.min = min;
	m_boundingBox.max = max;
}


void Model::ComputeBoudingBox()
{
	Vector3 minAABB = Vector3::One * FLT_MAX, maxAABB = Vector3::One * (-FLT_MAX);

	unsigned int meshIndex = 0;
	for (Mesh* mesh : m_meshes)
	{
		Vector3 subMeshMinAABB;
		memcpy(&subMeshMinAABB, &mesh->GetBoudingBox().min, sizeof(Vector3));

		Vector3 subMeshMaxAABB;
		memcpy(&subMeshMaxAABB, &mesh->GetBoudingBox().max, sizeof(Vector3));

		subMeshMinAABB = (subMeshMinAABB * m_meshesOffsets[meshIndex].scale + m_meshesOffsets[meshIndex].position);
		subMeshMaxAABB = (subMeshMaxAABB * m_meshesOffsets[meshIndex].scale + m_meshesOffsets[meshIndex].position);

		if (subMeshMinAABB.x < minAABB.x) minAABB.x = subMeshMinAABB.x;
		if (subMeshMinAABB.y < minAABB.y) minAABB.y = subMeshMinAABB.y;
		if (subMeshMinAABB.z < minAABB.z) minAABB.z = subMeshMinAABB.z;

		if (subMeshMaxAABB.x > maxAABB.x) maxAABB.x = subMeshMaxAABB.x;
		if (subMeshMaxAABB.y > maxAABB.y) maxAABB.y = subMeshMaxAABB.y;
		if (subMeshMaxAABB.z > maxAABB.z) maxAABB.z = subMeshMaxAABB.z;

		meshIndex++;
	}

	SetBoudingBox(minAABB, maxAABB);
}

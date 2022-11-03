#pragma once

#include <vector>

#include "Resources/Resource/Resource.hpp"
#include "Resources/Types.hpp"
#include "Resources/Animation/Bone.hpp"

#include "Generated/Mesh.rfkh.h"

class Material;
struct GPUMeshData;

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	Material* material;
	GPUMeshData* GPUData = nullptr;
};

struct BoundingBox
{
	Vector3 min;
	Vector3 max;
};

/**
@brief Mesh is a vertex list representing a 3D object
*/
class HY_CLASS() Mesh : public Resource
{
private:
	HY_METHOD(rfk::Instantiator)
		static rfk::UniquePtr<Mesh> defaultInstantiator(const HYGUID & uid);

protected:

	BoundingBox	  m_boundingBox;

public:
	std::vector<MeshData> subMeshes;

protected:
	Mesh(const HYGUID & uid, const std::string & name);

public:
	Mesh(const HYGUID& uid);

	virtual void LoadInGPUMemory() override;
	virtual void UnloadFromGPUMemory() override;

	virtual void Serialize() override;
	virtual void Deserialize() override;

	ENGINE_API const BoundingBox& GetBoudingBox() const;
	void SetBoudingBox(const Vector3& min, const Vector3& max);

	Mesh_GENERATED
};

File_Mesh_GENERATED

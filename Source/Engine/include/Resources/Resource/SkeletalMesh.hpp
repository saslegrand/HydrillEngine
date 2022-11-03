#pragma once

#include "Resources/Resource/Mesh.hpp"

#include "Generated/SkeletalMesh.rfkh.h"

struct GPUSkeletalData;

struct BoneInfo
{
	int id;
	Matrix4 offsetMatrix;
};

/**
@brief VertexBoneData stores the info related to skeleton for one vertex, for skinned mesh rendering
*/
struct VertexBoneData
{
	int boneIDs[MAX_BONE_INFLUENCE] = { 0 };
	float weights[MAX_BONE_INFLUENCE] = { 0.0f };
};


/**
@brief SkeletonData stores everything related to the bone armature
*/
struct SkeletalData
{
	std::unordered_map<std::string, BoneInfo> boneLink;
	std::vector<VertexBoneData> vertexBoneData;
	std::vector<Matrix4> boneTransforms;

	GPUSkeletalData* GPUData = nullptr;
};


class HY_CLASS() SkeletalMesh : public Mesh
{
private:
	HY_METHOD(rfk::Instantiator)
		static rfk::UniquePtr<SkeletalMesh> defaultInstantiator(const HYGUID& uid);

public:
	std::vector<SkeletalData> skeletonDatas;

	SkeletalMesh(const HYGUID& uid);

	virtual void LoadInGPUMemory() override;
	virtual void UnloadFromGPUMemory() override;

	virtual void Serialize() override;
	virtual void Deserialize() override;

	SkeletalMesh_GENERATED
};

File_SkeletalMesh_GENERATED
#pragma once


class	Material;
struct	MeshData;
class	Transform;
struct	SkeletalData;
class	MeshRenderPipeline;


/**
@brief Mesh instance that contains all need informations to render a mesh
*/
struct MeshInstance
{
	MeshInstance(MeshData* mesh, Material* material, Transform* transform);

	bool	isActive = false;
	MeshData* mesh         = nullptr;
	Material* material     = nullptr;
	Transform* transform   = nullptr;

	MeshRenderPipeline* savedPipeline = nullptr;
};


/**
@brief Skeletal mesh instance that contains all need informations to render a skeletal mesh
*/
struct SkeletalMeshInstance : public MeshInstance
{
	SkeletalMeshInstance(MeshData* mesh, Material* material, Transform* transform, SkeletalData* skeleton);

	SkeletalData* skeleton = nullptr;
};

#pragma once

#include <vector>
#include <map>

#include "Renderer/MeshRenderPipeline.hpp"
#include "Renderer/MeshInstance.hpp"

//	Forward declarations

class MeshUnlitShader;
class MeshLitShader;
class MeshPBRShader;

/**
@brief Mesh render pipeline, manage the rendering of meshes
*/
class TransparentRenderPipeline : public MeshRenderPipeline
{

public:

	struct BlendMesh
	{
		float distanceFromCamera = 0.0f;
		const MeshInstance* instance;
		bool skeletal = false;
	};

//	Variables

public:

	std::vector<const MeshInstance*> meshes;
	std::vector<const SkeletalMeshInstance*> skeletalMeshes;

	MeshUnlitShader* unlitShader = nullptr;
	MeshLitShader* litShader = nullptr;
	MeshPBRShader* pbrShader = nullptr;

//	Constructors & Destructors

public:

	TransparentRenderPipeline();
	~TransparentRenderPipeline();

//	Functions

private:


public:

	void Initialize() override;

	/**
	* @brief Render all mesh which were added
	*/
	void Render() override;

	void AddMeshInstance(MeshInstance* instance);
	void RemoveMeshInstance(MeshInstance* instance);

	void AddSkeletalMeshInstance(SkeletalMeshInstance* instance);
	void RemoveSkeletalMeshInstance(SkeletalMeshInstance* instance);

	std::vector<BlendMesh> SortMeshes();
};
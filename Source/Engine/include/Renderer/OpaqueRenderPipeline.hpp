#pragma once
#include <vector>
#include <map>

#include "Renderer/MeshRenderPipeline.hpp"

//	Forward declarations

class MeshUnlitShader;
class MeshLitShader;
class MeshPBRShader;

/**
@brief Mesh render pipeline, manage the rendering of meshes
*/
class OpaqueRenderPipeline : public MeshRenderPipeline
{
//	Variables

public:

	MeshUnlitShader* unlitShader = nullptr;
	MeshLitShader*	 litShader   = nullptr;
	MeshPBRShader*	 pbrShader   = nullptr;

//	Constructors & Destructors

public:

	OpaqueRenderPipeline();
	~OpaqueRenderPipeline();

//	Functions

private:


public:

	void Initialize() override;

	/**
	* @brief Render all mesh which were added
	*/
	void Render() override;


	/**
	@brief Register a mesh instances to this pipeline

	@param instance : Mesh instance to add
	*/
	void AddMeshInstance(MeshInstance* instance);

	/**
	@brief Remove given mesh instance froo this pipeline

	@param instance : Mesh instance to remove
	*/
	void RemoveMeshInstance(MeshInstance* instance);

	/**
	@brief Register a skeletal mesh instances to this pipeline

	@param mesh : Skeletal Mesh instance to add
	*/
	void AddSkeletalMeshInstance(SkeletalMeshInstance* instance);

	/**
	@brief Remove given skeletal mesh instance from this pipeline

	@param mesh : Skeletal Mesh instance to remove
	*/
	void RemoveSkeletalMeshInstance(SkeletalMeshInstance* instance);
};
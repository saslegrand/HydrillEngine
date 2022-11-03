#pragma once

#include "Renderer/RenderPipeline.hpp"

struct MeshInstance;
struct SkeletalMeshInstance;

/**
@brief abstract and base class of all render pipeline classes
*/
class MeshRenderPipeline : public RenderPipeline
{

//	Functions
public:

	/**
	@brief Register a mesh instances to this pipeline

	@param mesh : Mesh component instance to add
	*/
	virtual void AddMeshInstance(MeshInstance* instance) = 0;

	/**
	@brief Remove given mesh instance froo this pipeline

	@param mesh : Mesh component instance to remove
	*/
	virtual void RemoveMeshInstance(MeshInstance* instance) = 0;

	/**
	@brief Register a skeletal mesh instances to this pipeline

	@param mesh : Skeletal Mesh component instance to add
	*/
	virtual void AddSkeletalMeshInstance(SkeletalMeshInstance* skeletalMeshComp) = 0;

	/**
	@brief Remove given skeletal mesh instance from this pipeline

	@param mesh : Skeletal Mesh component instance to remove
	*/
	virtual void RemoveSkeletalMeshInstance(SkeletalMeshInstance* skeletalMeshComp) = 0;
};
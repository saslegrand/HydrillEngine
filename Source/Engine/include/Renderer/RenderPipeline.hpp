#pragma once

struct MeshInstance;
struct SkeletalMeshInstance;

/**
@brief abstract and base class of all render pipeline classes
*/
class RenderPipeline
{

//	Functions

public:

	/**
	@brief Initialize render pipeline
	*/
	virtual void Initialize() = 0;

	/**
	@brief Prerender function of the render pipeline
	*/
	virtual void PreRender() {};

	/**
	@brief Render function of the render pipeline
	*/
	virtual void Render() = 0;
};
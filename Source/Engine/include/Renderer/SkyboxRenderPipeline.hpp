#pragma once
#include <vector>
#include <map>

#include "Renderer/RenderPipeline.hpp"
#include "Renderer/Primitives/ShaderProgram.hpp"
#include "Renderer/Primitives/VertexArray.hpp"
#include "Renderer/Primitives/VertexBuffer.hpp"

class Skybox;
class Mesh;
class Camera;

/**
@brief Mesh render pipeline, manage the rendering of meshes
*/
class SkyboxRenderPipeline : public RenderPipeline
{
//	Variables

public:

	Skybox* attachedSkybox;
	Mesh*	cubeMesh;

	ShaderProgram shader;
	VertexArray cubeVAO;
	VertexBuffer cubeVBO;

//	Constructors & Destructors

public:

	SkyboxRenderPipeline();
	~SkyboxRenderPipeline();

//	Functions

private:
	void InitCube();

public:

	void Initialize() override;

	/**
	* @brief Render all mesh which were added
	*/
	void Render() override;
};

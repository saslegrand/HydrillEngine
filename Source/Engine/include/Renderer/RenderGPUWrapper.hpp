#pragma once
#include <deque>
#include <list>

#include "Renderer/GPUMeshData.hpp"
#include "Renderer/GPUSkeletalData.hpp"
#include "Renderer/GPUTextureData.hpp"

#include "Renderer/Primitives/GLTexture.hpp"
#include "Renderer/Primitives/GLCubeMap.hpp"
#include "Renderer/Primitives/VertexArray.hpp"
#include "Renderer/Primitives/VertexBuffer.hpp"
#include "Renderer/Primitives/ShaderProgram.hpp"
#include "Renderer/Primitives/GLTextureArray.hpp"

class Mesh;
class SkeletalMesh;
class Texture;
class Skybox;

struct VertexObject
{
	VertexArray  VAO;
	VertexBuffer VBO;
};


struct RenderGPUWrapper
{

//	Variables

private:

	VertexBuffer meshVBO;
	VertexBuffer skeletalMeshVBO;
	VertexBuffer skeletalDataVBO;

public:
	VertexArray  meshVAO;
	VertexArray  skeletalMeshVAO;

	std::list<GPUMeshData> GPUMeshDatas;
	std::list<GPUMeshData> GPUSkeletalMeshDatas;
	std::list<GPUSkeletalData> GPUSkeletalDatas;
	std::list<GPUTextureData> GPUTextureDatas;

	std::list<GLTexture>	   GeneratedTextures;

	GLTextureArray GeneratedTextureArray;
	GLTextureArray GeneratedDepthMapTextureArray;
	GLTextureArray GeneratedDepthCubeMapTextureArray;
	GLTextureArray GeneratedCascadeDepthMapTextureArray;

	std::list<GLCubeMap>	   GPUSkyboxDatas;

//	Functions

public:

	RenderGPUWrapper();
	~RenderGPUWrapper();

	/**
	@brief Create both VAO and VBO for mesh
	*/
	void CreateVertexObjects();

	//	Create datas functions

	/**
	@brief Send Mesh datas to GPU

	@param mesh : mesh to send
	*/
	void CreateMeshData(Mesh& mesh);

	/**
	@brief Send Skeletal Mesh datas to GPU

	@param skMesh : skeletal mesh to send
	*/
	void CreateSkeletalMeshData(SkeletalMesh& skMesh);

	/**
	@brief Send Texture datas to GPU

	@param text : texture to send
	*/
	void CreateTextureData(Texture& text);

	/**
	@brief Send Skybox datas to GPU

	@param skybox : skybox to send
	*/
	void CreateSkyboxData(Skybox& skybox);

	/**
	@brief Copy a Texture into a texture array

	@param text : texture to copy
	*/
	void CopyInTextureArray(Texture& text);

	//	Remove datas functions

	/**
	@brief Remove Mesh datas to GPU

	@param mesh : mesh to remove from GPU
	*/
	void RemoveMeshData(Mesh& mesh);

	/**
	@brief Remove Skeletal mesh datas to GPU

	@param skMesh : skeletal mesh to remove from GPU
	*/
	void RemoveSkeletalMeshData(SkeletalMesh& skMesh);

	/**
	@brief Remove Texture datas to GPU

	@param texture : texture to remove from GPU
	*/
	void RemoveTextureData(Texture& text);

	/**
	@brief Remove Skybox datas to GPU

	@param skybox : skybox to remove from GPU
	*/
	void RemoveSkyboxData(Skybox& skybox);

	/**
	@brief Remove the copy of a Texture from a texture array

	@param text : texture to remove
	*/
	void RemoveTextureArray(Texture& text);

};
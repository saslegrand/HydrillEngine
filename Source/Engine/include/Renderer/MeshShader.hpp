#pragma once

#include <vector>

#include "Renderer/Primitives/ShaderProgram.hpp"
#include "Renderer/MeshRenderPipeline.hpp"

struct MeshData;
struct SkeletalData;
struct MaterialData;
struct Matrix4;

class  Mesh;

struct PBRUniformsLocation
{
	int32_t matColor;
	int32_t packedMat1;
	int32_t packedMat2;
};

struct MeshPBRUniformsLocation : public PBRUniformsLocation
{
	int32_t matModel;
	int32_t matModelNormal;
};

struct SkMeshPBRUniformsLocation : public PBRUniformsLocation
{
	int32_t matModel;
	int32_t matModelNormal;
	int32_t matSkin;
};

/**
@brief Mesh Shader class can keep instance lists of meshes or skeletal meshes and can render them with less openGL calls
There are also single instances rendering functions that can be used if blending is enabled with transparency.
*/
class MeshShader : public MeshRenderPipeline
{
//	Variables

private:

	int m_offsetCount = 0;

protected:
	MeshPBRUniformsLocation		m_meshLocations;
	SkMeshPBRUniformsLocation	m_skMeshLocations;

	std::vector<const MeshInstance*> m_meshInstances;
	std::vector<const SkeletalMeshInstance*> m_skeletalMeshInstances;

	ShaderProgram m_meshShader;
	ShaderProgram m_skeletalMeshShader;

	static const MaterialData* m_defaultMat;

//	Constructors & Destructors

public:

	ENGINE_API MeshShader();
	~MeshShader();

//	Functions

protected :

	/**
	@brief Sens Materials data to the shader

	@param Material datas to send
	*/
	virtual void SendMaterialDatas(MaterialData& material, PBRUniformsLocation const& locations) const = 0;

public:


	/**
	@brief Bind mesh shader
	*/
	ENGINE_API void BindMeshShader() const;

	/**
	@brief Bind skeletal mesh shader
	*/
	ENGINE_API void BindSkeletalMeshShader() const;

	/**
	@brief unbind shader
	*/
	ENGINE_API void UnbindShader() const;

	/**
	@brief Clear all instances registered to this shader class
	*/
	ENGINE_API void ClearMeshInstances();

	/**
	@brief Register a mesh instances to this pipeline

	@param instance : Mesh component instance to add
	*/
	ENGINE_API void AddMeshInstance(MeshInstance* instance);

	/**
	@brief Remove given mesh instance froo this pipeline

	@param instance : Mesh component instance to remove
	*/
	ENGINE_API void RemoveMeshInstance(MeshInstance* instance);

	/**
	@brief Register a skeletal mesh instances to this pipeline

	@param instance : Skeletal Mesh component instance to add
	*/
	ENGINE_API void AddSkeletalMeshInstance(SkeletalMeshInstance* instance);

	/**
	@brief Remove given skeletal mesh instance from this pipeline

	@param instance : Skeletal Mesh component instance to remove
	*/
	ENGINE_API void RemoveSkeletalMeshInstance(SkeletalMeshInstance* instance);

	/**
	@brief Render a given mesh component using this pipeline shader, no need to bind VAO and Shader, 
	it is done in this function, that's why it is recommended to use this function only for blended meshes

	@param instance : Mesh to render
	*/
	ENGINE_API void RenderSingleInstance(const MeshInstance* instance) const;

	/**
	@brief Render a given skeletal mesh component using this pipeline shader, no need to bind VAO and Shader,
	it is done in this function, that's why it is recommended to use this function only for blended skeletal meshes

	@param instance : Skeletal mesh to render
	*/
	ENGINE_API void RenderSingleInstance(const SkeletalMeshInstance* instance) const;

	/**
	@brief Render all registered mesh and skeletal mesh instance with fewer openGL calls
	*/
	ENGINE_API void Render();

	/**
	@brief Set default material for all MeshShaders to use by default

	@param mat : default material to set
	*/
	ENGINE_API static void SetDefaultMaterial(const MaterialData* material);

	/**
	@brief Draw a mesh given a model, a mesh and an optionnal material. Shader and VAO must be bound before.

	@param model : world model matrix used to place the mesh in space
	@param mesh : mesh to render
	@param material : material to render the mesh with
	*/
	ENGINE_API virtual void DrawMesh(const Matrix4& model, const MeshData& mesh, const MaterialData* material) const = 0;

	/**
	@brief Draw a skeletal mesh given a model, a skeletal mesh, a skeletal data and an optionnal material. Shader and VAO must be bound before.

	@param model : world model matrix used to place the mesh in space
	@param mesh : skeletal mesh to render
	@param skeleton : skeleton data to send with the mesh
	@param material : material to render the skeletal mesh with
	*/
	ENGINE_API virtual void DrawSkeletalMesh(const Matrix4& model, const MeshData& mesh, const SkeletalData& skeleton, const MaterialData* material) const = 0;
};

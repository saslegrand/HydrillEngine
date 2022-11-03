#pragma once

#include "Renderer/MeshShader.hpp"

/**
@brief Mesh shader class with linked PBR shader, and its rendering functions
*/
class MeshPBRShader : public MeshShader
{
//	Functions

private:

	/**
	@brief Sens Materials data to the shader

	@param Material datas to send
	*/
	ENGINE_API void SendMaterialDatas(MaterialData& material, PBRUniformsLocation const& locations) const override;

public:

	/**
	@brief Initialize this shader class
	*/
	void Initialize() override;

	/**
	@brief Draw a mesh given a model, a mesh and an optionnal material. Shader and VAO must be bound before.

	@param model : world model matrix used to place the mesh in space
	@param mesh : mesh to render
	@param material : material to render the mesh with
	*/
	ENGINE_API void DrawMesh(const Matrix4& model, const MeshData& mesh, const MaterialData* material) const override;

	/**
	@brief Draw a skeletal mesh given a model, a skeletal mesh, a skeletal data and an optionnal material. Shader and VAO must be bound before.

	@param model : world model matrix used to place the mesh in space
	@param mesh : skeletal mesh to render
	@param skeleton : skeleton data to send with the mesh
	@param material : material to render the skeletal mesh with
	*/
	ENGINE_API void DrawSkeletalMesh(const Matrix4& model, const MeshData& mesh, const SkeletalData& skeleton, const MaterialData* material) const override;
};

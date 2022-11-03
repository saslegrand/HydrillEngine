#pragma once

#include <string>
#include <vector>
#include <mutex>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Tools/Flags.hpp"
#include "Resources/Parsers/ParserFlags.hpp"

class Model;
class Mesh;
class Texture;
class ResourcesManager;

class AssimpParser
{
public:
	/*
	@brief Try to load a model with its filepath

	@param model : Model&
	@param flags : int - Assimp Load Flags
	*/
	static bool LoadModel(Model& model, int flags);

	/*
	@brief Try to load a texture with its filepath

	@param texture : Texture &
	@param flags : int - STB Load Flags
	@param shouldFreeDara : bool - should free the texture data
	*/
	static bool LoadTexture(Texture& texture, const Flags<EImageSTB>& flags, bool shouldFreeData);

	/*
	@brief Free the texture s

	@param texture : Texture&
	*/
	static void FreeTexture(Texture& texture);

private:
	static void ProcessMaterials(Model& model, const aiScene* scene);

	static void ProcessNode(Model& model, aiNode* node, const aiScene* scene, const std::string& parentID);
	static void ProcessMesh(Model& model, Mesh* mesh, aiMesh* aimesh, const aiScene* scene);
	
	static void LoadMaterialTextures(Texture** texture, Model& model, aiMaterial* mat, aiTextureType type, const aiScene* scene, Texture* defaultTexture);
};

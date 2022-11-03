#include "Resources/Parsers/AssimpParser.hpp"

#include <glad/gl.h>
#include <stb_image/stb_image.h>
#include <stb_image/stb_image_write.h>
#include <assimp/postprocess.h>
#include <nlohmann/json.hpp>

#include "Resources/Types.hpp"
#include "Resources/Resource/Texture.hpp"
#include "Resources/Resource/Material.hpp"
#include "Resources/Resource/Model.hpp"
#include "Resources/Resource/SkeletalMesh.hpp"
#include "ECS/MeshComponent.hpp"
#include "Tools/StringHelper.hpp"
#include "Tools/SerializationUtils.hpp"
#include "Tools/Flags.hpp"
#include "Core/Logger.hpp"

#include "EngineContext.hpp"
#include "Resources/ResourcesManager.hpp"
#include "Resources/Loaders/ResourcesLoader.hpp"

#include "Renderer/RenderSystem.hpp"

#include "Generated/ParserFlags.rfks.h"


bool AssimpParser::LoadModel(Model& model, int flags)
{
	// Importer aitextures and aiMeshes are used in another thread
	// This is done to avoid copying the resources data
	std::shared_ptr<Assimp::Importer> importer = std::make_shared<Assimp::Importer>();

	ResourcesTaskPool& taskPool = EngineContext::Instance().resourcesManager->GetTaskPool();
	// Kill of the importer instance after tasks completion (copy the shared_ptr to keep alive the importer instance)
	taskPool.AddSingleThreadTask([importer](){});

	importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false); // Temporary Fix for debug. Animations might need them to be treated if helper nodes

	// Import scene
	const aiScene* scene = importer->ReadFile(model.m_originalPath, static_cast<unsigned int>(flags));
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Logger::Error("Error ASSIMP model loader : " + std::string(importer->GetErrorString()));
		return false;
	}

	// Load model
	model.GetJsonGraph().clear();
	ProcessMaterials(model, scene);
	ProcessNode(model, scene->mRootNode, scene, "0");

	taskPool.AddSingleThreadTask([&model]() { model.ComputeBoudingBox(); });
	taskPool.AddSingleThreadTask([&model]() { ResourcesLoader::CreateResourceFiles(&model); });

	return true;
}

void AssimpParser::ProcessMaterials(Model& model, const aiScene* scene)
{
	ResourcesManager* RM = EngineContext::Instance().resourcesManager;

	for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* material = scene->mMaterials[i];

		Material* mat = RM->CreateResource<Material>();
		std::string matPath = StringHelper::GetDirectory(model.GetFilepath()) + std::string(material->GetName().C_Str()) + ".mat";
		mat->SetFileInfo(matPath);

		aiColor3D ambientColor;
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
		mat->data.ambientColor = { ambientColor.r, ambientColor.g, ambientColor.b };

		aiColor3D diffuseColor;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		mat->data.diffuseColor = { diffuseColor.r, diffuseColor.g, diffuseColor.b };

		aiColor3D specularColor;
		material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
		mat->data.specularColor = { specularColor.r, specularColor.g, specularColor.b };

		aiColor3D emissiveColor;
		material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
		mat->data.emissiveColor = { emissiveColor.r, emissiveColor.g, emissiveColor.b };

		Texture* whiteTexture = static_cast<Texture*>(RM->GetResourceByFilename("White.texture"));
		Texture* blackTexture = static_cast<Texture*>(RM->GetResourceByFilename("Black.texture"));

		LoadMaterialTextures(&mat->data.ambientTexture, model, material, aiTextureType_AMBIENT, scene, whiteTexture);
		LoadMaterialTextures(&mat->data.diffuseTexture, model, material, aiTextureType_DIFFUSE, scene, whiteTexture);
		LoadMaterialTextures(&mat->data.specularTexture, model, material, aiTextureType_SPECULAR, scene, whiteTexture);
		LoadMaterialTextures(&mat->data.emissiveTexture, model, material, aiTextureType_EMISSIVE, scene, whiteTexture);
		LoadMaterialTextures(&mat->data.metallicTexture, model, material, aiTextureType_METALNESS, scene, whiteTexture);
		LoadMaterialTextures(&mat->data.roughnessTexture, model, material, aiTextureType_DIFFUSE_ROUGHNESS, scene, whiteTexture);
		LoadMaterialTextures(&mat->data.normalTexture, model, material, aiTextureType_NORMALS, scene, nullptr);
		LoadMaterialTextures(&mat->data.aoTexture, model, material, aiTextureType_AMBIENT_OCCLUSION, scene, whiteTexture);

		RM->CreateResourceFiles(mat);
		model.AddMaterial(*mat);
	}
}

void AssimpParser::ProcessNode(Model& model, aiNode* node, const aiScene* scene, const std::string& parentID)
{
	// Create model node prefab
	json& root = model.GetJsonGraph();
	std::string id = std::string(HYGUID::NewGUID());
	json& goJson = root[id];
	json& goInternal = goJson["Internal"];
	json& goComponents = goJson["Components"];

	goInternal["m_parent"] = parentID;
	goInternal["m_name"] = scene->mRootNode == node ? StringHelper::GetFileNameWithoutExtension(model.GetFilename()) : node->mName.C_Str();

	aiQuaternion rotation;
	aiVector3D position, scale;
	node->mTransformation.Decompose(scale, rotation, position);
	Quaternion rot = { rotation.x, rotation.y, rotation.z, rotation.w };

	TransformData transformOffset;
	transformOffset.position = { position.x, position.y, position.z };
	transformOffset.scale = { scale.x, scale.y, scale.z };
	transformOffset.rotation = rot;
	transformOffset.ComputeLocalTRS();

	json& goTransformJson = goInternal["transform"];
	Serialization::SetContainer<float>(goTransformJson["position"], position);
	Serialization::SetContainer<float>(goTransformJson["rotation"], rot);
	Serialization::SetContainer<float>(goTransformJson["scale"], scale);
	bool isStatic = false;
	Serialization::SetContainer<bool>(goTransformJson["isStatic"], isStatic);

	ResourcesManager* RM = EngineContext::Instance().resourcesManager;

	Mesh* modelMesh = nullptr;
	bool isSkeletal = false;

	if (node->mNumMeshes > 0) // CREATE MESH RESOURCE
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];
		isSkeletal = mesh->HasBones();
		std::string meshName = mesh->mName.C_Str();

		if (isSkeletal)
		{
			modelMesh = RM->CreateResource<SkeletalMesh>();
			modelMesh->SetFileInfo(StringHelper::GetDirectory(model.GetFilepath()) + meshName + ".skmesh");
			//modelMesh->SetFilename(meshName + ".skmesh");
			//modelMesh->SetFilepath(StringHelper::GetDirectory(model.GetFilepath()) + meshName + ".skmesh");
		}
		else
		{
			modelMesh = RM->CreateResource<Mesh>();
			modelMesh->SetFileInfo(StringHelper::GetDirectory(model.GetFilepath()) + meshName + ".mesh");
			//modelMesh->SetFilename(meshName + ".mesh");
			//modelMesh->SetFilepath(StringHelper::GetDirectory(model.GetFilepath()) + meshName + ".mesh");
		}
	}

	ResourcesTaskPool& taskPool = RM->GetTaskPool();

	Vector3 minAABB = Vector3::One * FLT_MAX, maxAABB = Vector3::One * (-FLT_MAX);

	// Process all the node's mesh
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		
		Vector3 subMeshMinAABB;
		memcpy(&subMeshMinAABB, &mesh->mAABB.mMin, sizeof(Vector3));

		Vector3 subMeshMaxAABB;
		memcpy(&subMeshMaxAABB, &mesh->mAABB.mMax, sizeof(Vector3));

		if (subMeshMinAABB.x < minAABB.x) minAABB.x = subMeshMinAABB.x;
		if (subMeshMinAABB.y < minAABB.y) minAABB.y = subMeshMinAABB.y;
		if (subMeshMinAABB.z < minAABB.z) minAABB.z = subMeshMinAABB.z;

		if (subMeshMaxAABB.x > maxAABB.x) maxAABB.x = subMeshMaxAABB.x;
		if (subMeshMaxAABB.y > maxAABB.y) maxAABB.y = subMeshMaxAABB.y;
		if (subMeshMaxAABB.z > maxAABB.z) maxAABB.z = subMeshMaxAABB.z;

		taskPool.AddMultiThreadTask([=, &model]() {
			ProcessMesh(model, modelMesh, mesh, scene);
			});
	}

	if (modelMesh)
	{
		modelMesh->SetBoudingBox(minAABB, maxAABB);

		taskPool.AddSingleThreadTask([modelMesh, &model, &goComponents, isSkeletal, transformOffset]() {
			// Add the mesh to the model
			model.AddMesh(modelMesh, transformOffset);

			// Serialize in component
			json& meshCompoJson = goComponents[std::string(HYGUID::NewGUID())];
			meshCompoJson["m_name"] = isSkeletal ? "SkeletalMeshComponent" : "MeshComponent";
			meshCompoJson["m_mesh"] = std::string(modelMesh->GetUID());

			json& materialsJonFile = meshCompoJson["materials"];
			for (uint32_t i = 0; i < modelMesh->subMeshes.size(); ++i)
			{
				MeshData& subMesh = modelMesh->subMeshes[i];

				std::string matName = "v" + std::to_string(i);
				materialsJonFile[matName] = std::string(subMesh.material->GetUID());
			}

			// Create files and GPUData
			ResourcesLoader::CreateResourceFiles(modelMesh);
			modelMesh->LoadInGPUMemory();

			// Clear sub meshes data
			for (auto& subMesh : modelMesh->subMeshes)
				subMesh.vertices.clear();
			});
	}

	// Then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		ProcessNode(model, node->mChildren[i], scene, id);
}

void AssimpParser::ProcessMesh(Model& model, Mesh* mesh, aiMesh* aimesh, const aiScene* scene)
{
	bool isSkeletal = aimesh->HasBones();
	SkeletalMesh* skMesh = static_cast<SkeletalMesh*>(mesh);
	if (skMesh == nullptr) isSkeletal = false;

	MeshData data;
	SkeletalData skeletalData;

	for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
	{
		Vector3 v = Vector3(0, 0, 0);
		Vector3 n = Vector3(0, 0, 0);
		Vector2 uv = Vector2(0, 0);
		Vector3 tan = Vector3(0, 0, 0);
		Vector3 bit = Vector3(0, 0, 0);

		v.x = aimesh->mVertices[i].x;
		v.y = aimesh->mVertices[i].y;
		v.z = aimesh->mVertices[i].z;

		if (aimesh->HasNormals())
		{
			n.x = aimesh->mNormals[i].x;
			n.y = aimesh->mNormals[i].y;
			n.z = aimesh->mNormals[i].z;
		}
		if (aimesh->mTextureCoords[0])
		{
			uv.x = aimesh->mTextureCoords[0][i].x;
			uv.y = aimesh->mTextureCoords[0][i].y;
		}
		if (aimesh->HasTangentsAndBitangents())
		{
			tan.x = aimesh->mTangents[i].x;
			tan.y = aimesh->mTangents[i].y;
			tan.z = aimesh->mTangents[i].z;

			bit.x = aimesh->mBitangents[i].x;
			bit.y = aimesh->mBitangents[i].y;
			bit.z = aimesh->mBitangents[i].z;
		}

		Vertex vertex;
		vertex.position = v;
		vertex.normal = n;
		vertex.uvs = uv;
		vertex.tangent = tan;

		data.vertices.push_back(vertex);

		if (isSkeletal)
		{
			VertexBoneData vBoneData;
			for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
			{
				vBoneData.boneIDs[i] = -1;
				vBoneData.weights[i] = 0;
			}
			skeletalData.vertexBoneData.emplace_back(vBoneData);
		}
	}

	if (isSkeletal)
	{
		for (unsigned int i = 0; i < aimesh->mNumBones; ++i)
		{
			int boneIdx = i;
			aiBone* aibone = aimesh->mBones[i];
			std::string boneName = aibone->mName.data;

			aiMatrix4x4 m = aibone->mOffsetMatrix;
			Matrix4 offsetMat = { m.a1, m.b1, m.c1, m.d1,
								  m.a2, m.b2, m.c2, m.d2,
								  m.a3, m.b3, m.c3, m.d3,
								  m.a4, m.b4, m.c4, m.d4 };

			skeletalData.boneLink[boneName] = { boneIdx, offsetMat };
			skeletalData.boneTransforms.push_back(Matrix4::Identity);

			for (unsigned int weightIdx = 0; weightIdx < aibone->mNumWeights; ++weightIdx)
			{
				unsigned int vertexID = aibone->mWeights[weightIdx].mVertexId;
				float weight = aibone->mWeights[weightIdx].mWeight;

				VertexBoneData& vertexBoneData = skeletalData.vertexBoneData[vertexID];
				
				for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
				{
					if (vertexBoneData.boneIDs[i] < 0)
					{
						vertexBoneData.boneIDs[i] = boneIdx;
						vertexBoneData.weights[i] = weight;
						break;
					}
				}
			}
		}
	}

	for (unsigned int faceID = 0; faceID < aimesh->mNumFaces; ++faceID)
	{
		aiFace& face = aimesh->mFaces[faceID];
		for (size_t indexID = 0; indexID < 3; ++indexID)
		{
			data.indices.push_back(face.mIndices[indexID]);
		}
	}

	unsigned int matIDX = aimesh->mMaterialIndex;
	if (matIDX >= 0 && matIDX < model.GetMaterials().size())
	{
		data.material = model.GetMaterials()[matIDX];;
	}

	mesh->subMeshes.push_back(data);
	if (isSkeletal)
		skMesh->skeletonDatas.push_back(skeletalData);
}

// Private function for getting GLint internal format according to desired channel number
GLint GetGLImageInternalFormat(int desiredChannels)
{
	const static GLint GLImageInternalFormat[] =
	{
		-1, // 0 Channels, unused
		GL_RED,
		GL_RG,
		GL_RGB,
		GL_RGBA,
	};

	if (desiredChannels < 0 || desiredChannels > 4)
		return GLImageInternalFormat[0];

	return GLImageInternalFormat[desiredChannels];
}

// Private function for getting GLint format according to desired channel number
GLint GetGLImageFormat(int desiredChannels)
{
	const static GLint GLImageFormat[] =
	{
		-1, // 0 Channels, unused
		GL_RED,
		GL_RG,
		GL_RGB,
		GL_RGBA,
	};

	if (desiredChannels < 0 || desiredChannels > 4)
		return GLImageFormat[0];

	return GLImageFormat[desiredChannels];
}

void AssimpParser::LoadMaterialTextures(Texture** texture, Model& model, aiMaterial* mat, aiTextureType type, const aiScene* scene, Texture*  defaultTexture)
{
	ResourcesManager* RM = EngineContext::Instance().resourcesManager;

	model;

	aiString aiTexturePath;
	if (mat->Get(AI_MATKEY_TEXTURE(type, 0), aiTexturePath) == AI_SUCCESS)
	{
		std::string modelDirectory = StringHelper::GetDirectory(model.GetFilepath());
		std::string textureFilename = StringHelper::GetFileNameWithoutExtension(StringHelper::GetFileNameFromPath(aiTexturePath.C_Str()));

		std::string texturePath = modelDirectory + textureFilename + ".texture";
		std::string subTexturePath = modelDirectory + R"(textures\)" + textureFilename + ".texture";

		{
			auto TryGetTexture = [RM, texture](const std::string& path)
			{
				Resource* existing = nullptr;
				if (existing = RM->GetResourceByPath(path))
				{
					*texture = static_cast<Texture*>(existing);
					return true;
				}

				return false;
			};

			// Check if the texture is already loaded (file root and textures dir)
			if (TryGetTexture(texturePath) ||
				TryGetTexture(subTexturePath))
				return;
		}

		const aiTexture* aiTexture = scene->GetEmbeddedTexture(aiTexturePath.C_Str());
		if (aiTexture && aiTexture->pcData) // embedded texture
		{
			*texture = RM->CreateResource<Texture>();

			Texture* texturePtr = *texture;
			texturePtr->originalPath = StringHelper::GetFilePathWithoutExtension(texturePath) + ".png"; // we will write it on disk as a .png
			texturePtr->SetFileInfo(texturePath);

			RM->GetTaskPool().AddMultiThreadTask([=]() {

				// Create PNG file
				{
					stbi_set_flip_vertically_on_load(false);

					int channelsCount = 0;
					int bufferLen = aiTexture->mHeight == 0 ? aiTexture->mWidth : aiTexture->mWidth * aiTexture->mHeight;

					texturePtr->data.data = stbi_load_from_memory(&aiTexture->pcData->b, bufferLen, &(texturePtr->data.width), &(texturePtr->data.height), &channelsCount, 0);

					if (texturePtr->data.data == nullptr)
					{
						RM->DeleteResource((*texture)->GetUID());

						*texture = defaultTexture;

						return;
					}

					texturePtr->data.channels = channelsCount;
					texturePtr->data.format = GetGLImageFormat(channelsCount);
					texturePtr->data.internalFormat = GetGLImageInternalFormat(channelsCount);

					// Write the texture on disk so we don't have to parse again to get it back
					stbi_write_png(texturePtr->originalPath.c_str(), texturePtr->data.width, texturePtr->data.height, channelsCount, texturePtr->data.data, texturePtr->data.width * channelsCount);
					stbi_image_free(texturePtr->data.data);
				}

				// Load texture for storage, texture is free in the LoadInGpuMemory
				if (!AssimpParser::LoadTexture(*texturePtr, texturePtr->GetLoadingFlags(), false))
				{
					FreeTexture(*texturePtr);
					RM->DeleteResource((*texture)->GetUID());

					*texture = defaultTexture;

					return;
				}

				// Create texture files
				ResourcesLoader::CreateResourceFiles(texturePtr);

				// Add loadGpu task
				RM->GetTaskPool().AddSingleThreadTask([texturePtr]() { texturePtr->LoadInGPUMemory(); });
				});

			return;
		}
		else // regular texture
		{
			RM->GetTaskPool().AddMultiThreadTask([=]() {

				Resource* res = nullptr;

				Logger::Warning(texturePath);

				auto TryLoadTexture = [RM, texture](const std::string& path) {

					RM->ImportResource<Texture>(path);
					Resource* res = RM->GetResourceByPath(path);

					if (res != nullptr)
					{
						*texture = static_cast<Texture*>(res);
						return true;
					}

					return false;
				};

				if (TryLoadTexture(texturePath) ||
					TryLoadTexture(subTexturePath))
					return;

				Logger::Warning("AssimpParser - Texture '" + texturePath + "' (root and textures dir) not found");

				*texture = defaultTexture;
				}); // Reload the texture from new file

			return;
		}
	}

	if (type == aiTextureType_NORMALS)
		return;

	*texture = defaultTexture;
}

void AssimpParser::FreeTexture(Texture& texture)
{
	if (texture.data.data != nullptr)
		stbi_image_free(texture.data.data);
}

bool AssimpParser::LoadTexture(Texture& texture, const Flags<EImageSTB>& flags, bool shouldFreeData)
{
	int DesiredChannels = 0;
	int Channels = 0;
	if (flags.TestBit(EImageSTB::IMG_FORCE_GREY))
	{
		DesiredChannels = STBI_grey;
		Channels = 1;
	}
	if (flags.TestBit(EImageSTB::IMG_FORCE_GREY_ALPHA))
	{
		DesiredChannels = STBI_grey_alpha;
		Channels = 2;
	}
	if (flags.TestBit(EImageSTB::IMG_FORCE_RGB))
	{
		DesiredChannels = STBI_rgb;
		Channels = 3;
	}
	if (flags.TestBit(EImageSTB::IMG_FORCE_RGBA))
	{
		DesiredChannels = STBI_rgb_alpha;
		Channels = 4;
	}

	stbi_set_flip_vertically_on_load(flags.TestBit(EImageSTB::IMG_FLIP));

	bool is16 = stbi_is_16_bit(texture.originalPath.c_str());

	if (is16)
	{
		texture.data.data = reinterpret_cast<unsigned char*>(stbi_load_16(texture.originalPath.c_str(), &texture.data.width, &texture.data.height, (DesiredChannels == 0) ? &Channels : nullptr, DesiredChannels));
		texture.data.type = GL_UNSIGNED_SHORT;
	}
	else
	{
		texture.data.data = stbi_load(texture.originalPath.c_str(), &texture.data.width, &texture.data.height, (DesiredChannels == 0) ? &Channels : nullptr, DesiredChannels);
		texture.data.type = GL_UNSIGNED_BYTE;
	}

	if (texture.data.data == nullptr)
	{
		return false;
	}

	texture.data.channels = Channels;
	texture.data.format = GetGLImageFormat(Channels);
	texture.data.internalFormat = GetGLImageInternalFormat(Channels);

	if (shouldFreeData)
		FreeTexture(texture);

	return true;
}

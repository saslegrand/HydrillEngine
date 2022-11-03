#include "Renderer/RenderGPUWrapper.hpp"

#include <glad/gl.h>

#include "Resources/Resource/Texture.hpp"
#include "Resources/Resource/Skybox.hpp"
#include "Resources/Resource/Mesh.hpp"
#include "Resources/Resource/SkeletalMesh.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Core/Logger.hpp"

constexpr GLsizeiptr MaxVboSize = 10000000 * sizeof(Vertex);

TextureArrayConfig shadowMapConfig =
{ 
	GL_TEXTURE_2D_ARRAY,
	Max_Lights_Count, 1024,1024,
	GL_DEPTH_COMPONENT32F,
	GL_DEPTH_COMPONENT,
	GL_FLOAT,
	GL_NEAREST,
	GL_NEAREST,
	GL_CLAMP_TO_EDGE,
	{0.f,0.f,0.f,0.f} 
};

TextureArrayConfig shadowCubeMapConfig =
{
	GL_TEXTURE_CUBE_MAP_ARRAY,
	6 * Max_Lights_Count, 1024,1024,
	GL_DEPTH_COMPONENT32F,
	GL_DEPTH_COMPONENT,
	GL_FLOAT,
	GL_NEAREST,
	GL_NEAREST,
	GL_CLAMP_TO_EDGE,
	{0.f,0.f,0.f,0.f}
};

TextureArrayConfig cascadeShadowMapConfig =
{
	GL_TEXTURE_2D_ARRAY,
	5 * Max_Lights_Count, 1024,1024,
	GL_DEPTH_COMPONENT16,
	GL_DEPTH_COMPONENT,
	GL_SHORT,
	GL_NEAREST,
	GL_NEAREST,
	GL_CLAMP_TO_EDGE,
	{1.f,1.f,1.f,1.f}
};


RenderGPUWrapper::RenderGPUWrapper() :
	GeneratedTextureArray(),
	GeneratedDepthMapTextureArray(shadowMapConfig),
	GeneratedDepthCubeMapTextureArray(shadowCubeMapConfig),
	GeneratedCascadeDepthMapTextureArray(cascadeShadowMapConfig)
{

}

RenderGPUWrapper::~RenderGPUWrapper()
{
	GPUMeshDatas.clear();
	GPUTextureDatas.clear();
	GPUSkyboxDatas.clear();
}

void RenderGPUWrapper::CreateVertexObjects()
{

	//	Meshes VAO & VBO setting
	{
		//	Meshes Vertex Array
		GLuint vao = meshVAO.GetID();

		// Allocate vertex buffer storage (empty)
		glNamedBufferStorage(meshVBO.GetID(), MaxVboSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Link array and buffer (binding 0)
		glVertexArrayVertexBuffer(vao, 0, meshVBO.GetID(), 0, sizeof(Vertex));

		// Enable attributes
		glEnableVertexArrayAttrib(vao, 0);	//	Position
		glEnableVertexArrayAttrib(vao, 1);	//	UVs
		glEnableVertexArrayAttrib(vao, 2);	//	Normal
		glEnableVertexArrayAttrib(vao, 3);	//	Tangent

		// Bind vao attributes to the vbo (from binding index)
		glVertexArrayAttribBinding(vao, 0, 0);
		glVertexArrayAttribBinding(vao, 1, 0);
		glVertexArrayAttribBinding(vao, 2, 0);
		glVertexArrayAttribBinding(vao, 3, 0);

		// Set attributes format
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
		glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uvs));
		glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
		glVertexArrayAttribFormat(vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
	}

	//	Skeletal Mesh VAO & VBO setting
	{
		//	Meshes Vertex Array
		GLuint vao = skeletalMeshVAO.GetID();

		//	SKELETAL MESH VERTEX BUFFER
		//	---------------------------
		
		// Allocate vertex buffer storage (empty)
		glNamedBufferStorage(skeletalMeshVBO.GetID(), MaxVboSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Link array and buffer (binding 0)
		glVertexArrayVertexBuffer(vao, 0, skeletalMeshVBO.GetID(), 0, sizeof(Vertex));

		// Enable attributes
		glEnableVertexArrayAttrib(vao, 0);	//	Position
		glEnableVertexArrayAttrib(vao, 1);	//	UVs
		glEnableVertexArrayAttrib(vao, 2);	//	Normal
		glEnableVertexArrayAttrib(vao, 3);	//	Tangent

		// Bind vao attributes to the vbo (from binding index)
		glVertexArrayAttribBinding(vao, 0, 0);
		glVertexArrayAttribBinding(vao, 1, 0);
		glVertexArrayAttribBinding(vao, 2, 0);
		glVertexArrayAttribBinding(vao, 3, 0);

		// Set attributes format
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
		glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uvs));
		glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
		glVertexArrayAttribFormat(vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));


		//	SKELETAL DATA VERTEX BUFFER
		//	---------------------------

		glNamedBufferStorage(skeletalDataVBO.GetID(), MaxVboSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

		glVertexArrayVertexBuffer(vao, 1, skeletalDataVBO.GetID(), 0, sizeof(VertexBoneData));

		glEnableVertexArrayAttrib(vao, 4);	//	BoneIndices
		glEnableVertexArrayAttrib(vao, 5);	//	BoneWeights

		glVertexArrayAttribBinding(vao, 4, 1);
		glVertexArrayAttribBinding(vao, 5, 1);

		glVertexArrayAttribFormat(vao, 4, MAX_BONE_INFLUENCE, GL_INT, GL_FALSE, offsetof(VertexBoneData, boneIDs));
		glVertexArrayAttribFormat(vao, 5, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, offsetof(VertexBoneData, weights));
	}
}


#pragma region CREATE_GPU_DATA
void RenderGPUWrapper::CreateMeshData(Mesh& mesh)
{
	for (MeshData& data : mesh.subMeshes)
	{
		if (data.GPUData != nullptr) continue;

		// Get mesh vertices size and new vbo size
		unsigned int size = static_cast<unsigned int>(data.vertices.size());
		unsigned int newVBOSize = (meshVBO.size + size) * sizeof(Vertex);

		// Check if the vertex buffer is enough large to contain the new mesh
		if (newVBOSize < MaxVboSize)
		{
			// Add new gpu mesh data
			GPUMeshData& gpuData = GPUMeshDatas.emplace_back();

			gpuData.offset = meshVBO.size;
			gpuData.size = size * sizeof(Vertex);

			data.GPUData = &gpuData;

			// Add the new mesh vertices data in the vertex buffer
			glNamedBufferSubData(meshVBO.GetID(), meshVBO.size * sizeof(Vertex), gpuData.size, data.vertices.data());

			meshVBO.size += size;
		}
	}
}



void RenderGPUWrapper::CreateSkeletalMeshData(SkeletalMesh& skMesh)
{
	for (MeshData& data : skMesh.subMeshes)
	{
		if (data.GPUData != nullptr) continue;

		// Get mesh vertices size and new vbo size
		unsigned int size = static_cast<unsigned int>(data.vertices.size());
		unsigned int newVBOSize = (skeletalMeshVBO.size + size) * sizeof(Vertex);

		// Check if the vertex buffer is enough large to contain the new mesh
		if (newVBOSize < MaxVboSize)
		{
			// Add new gpu mesh data
			GPUMeshData& gpuData = GPUSkeletalMeshDatas.emplace_back();

			gpuData.offset = skeletalMeshVBO.size;
			gpuData.size = size * sizeof(Vertex);

			data.GPUData = &gpuData;

			// Add the new mesh vertices data in the vertex buffer
			glNamedBufferSubData(skeletalMeshVBO.GetID(), skeletalMeshVBO.size * (sizeof(Vertex)), gpuData.size, data.vertices.data());

			skeletalMeshVBO.size += size;
		}
	}

	//	TODO : TEST IF IT IS WORKING OR NOT v v v

	for (SkeletalData& data : skMesh.skeletonDatas)
	{
		if (data.GPUData != nullptr) continue;

		// Get mesh vertices size and new vbo size
		unsigned int size = static_cast<unsigned int>(data.vertexBoneData.size());
		unsigned int newVBOSize = (skeletalDataVBO.size + size) * sizeof(VertexBoneData);

		// Check if the vertex buffer is enough large to contain the new mesh
		if (newVBOSize < MaxVboSize)
		{
			// Add new gpu mesh data
			GPUSkeletalData& gpuData = GPUSkeletalDatas.emplace_back();

			gpuData.offset = skeletalDataVBO.size;
			gpuData.size = size * sizeof(VertexBoneData);

			data.GPUData = &gpuData;

			glNamedBufferSubData(skeletalDataVBO.GetID(), skeletalDataVBO.size * sizeof(VertexBoneData), gpuData.size, data.vertexBoneData.data());

			skeletalDataVBO.size += size;
		}
	}
}



void RenderGPUWrapper::CreateTextureData(Texture& text)
{
	GPUTextureData& gpuData = GPUTextureDatas.emplace_back();
	GLTexture & generatedTexture = GeneratedTextures.emplace_back();
	gpuData.generatedTexture = &generatedTexture;

	text.GPUData = &gpuData;

	generatedTexture.Generate(text);
}



void RenderGPUWrapper::CreateSkyboxData(Skybox& skybox)
{
	GLCubeMap& gpuData = GPUSkyboxDatas.emplace_back();
	skybox.GPUData = &gpuData;

	gpuData.Generate(skybox);
}


void RenderGPUWrapper::CopyInTextureArray(Texture& text)
{
	GeneratedTextureArray.Add(text);
}

#pragma endregion


#pragma region REMOVE_GPU_DATA


void RenderGPUWrapper::RemoveMeshData(Mesh& mesh)
{
	unsigned int invalidGPUDataCount = 0;
	unsigned int notFoundGPUDataCount = 0;

	for (MeshData& data : mesh.subMeshes)
	{
		if (data.GPUData == nullptr)
		{
			invalidGPUDataCount++;
			continue;
		}

		bool found = false;

		GPUMeshData const* gpuMeshData = data.GPUData;
		auto gpuMeshIt = std::find_if(GPUMeshDatas.begin(), GPUMeshDatas.end(),
			[gpuMeshData](GPUMeshData const& gpuData) { return &gpuData == gpuMeshData; });

		if (gpuMeshIt != GPUMeshDatas.end())
		{
			found = true;

			// Free old space
			glNamedBufferSubData(meshVBO.GetID(), data.GPUData->offset, data.GPUData->size, nullptr);

			GPUMeshDatas.erase(gpuMeshIt);
		}

		//for (auto const& gpuData : GPUMeshDatas)//size_t i = 0; i < GPUMeshDatas.size(); i++)
		//{
		//	if (&gpuData == data.GPUData)
		//	{
		//		found = true;

		//		// Free old space
		//		glNamedBufferSubData(meshVBO.GetID(), data.GPUData->offset, data.GPUData->size, nullptr);

		//		GPUMeshDatas.remove(gpuData);

		//		//	Then move to next iteration
		//		break;
		//	}
		//}

		if (!found) notFoundGPUDataCount++;
	}

	if (invalidGPUDataCount)  Logger::Warning("RenderGPUWrapper - RemoveMeshData : GPUMeshData(s) not valid in " + std::to_string(invalidGPUDataCount) + " submesh(es) (Mesh : " + mesh.GetName() + ")");
	if (notFoundGPUDataCount) Logger::Warning("RenderGPUWrapper - RemoveMeshData : GPUMeshData(s) from " + std::to_string(notFoundGPUDataCount) + " submesh(es) (Mesh : " + mesh.GetName() + ") couldn't be found");
}



void RenderGPUWrapper::RemoveSkeletalMeshData(SkeletalMesh& skMesh)
{
	unsigned int invalidGPUDataCount = 0, notFoundGPUDataCount = 0;

	for (MeshData& data : skMesh.subMeshes)
	{
		if (data.GPUData == nullptr)
		{
			invalidGPUDataCount++;
			continue;
		}

		bool found = false;

		GPUMeshData const* gpuMeshData = data.GPUData;
		auto gpuMeshIt = std::find_if(GPUMeshDatas.begin(), GPUMeshDatas.end(),
			[gpuMeshData](GPUMeshData const& gpuData) { return &gpuData == gpuMeshData; });

		if (gpuMeshIt != GPUMeshDatas.end())
		{
			found = true;

			// Free old space
			glNamedBufferSubData(skeletalMeshVBO.GetID(), data.GPUData->offset, data.GPUData->size, nullptr);

			//	Then remove datas from the list
			GPUMeshDatas.erase(gpuMeshIt);
		}

		if (!found) notFoundGPUDataCount++;
	}

	if (invalidGPUDataCount)  Logger::Warning("RenderGPUWrapper - RemoveSkeletalMeshData : GPUMeshData(s) not valid in " + std::to_string(invalidGPUDataCount) + " submesh(es) (Skeletal mesh : " + skMesh.GetName() + ")");
	if (notFoundGPUDataCount) Logger::Warning("RenderGPUWrapper - RemoveSkeletalMeshData : GPUMeshData(s) from " + std::to_string(notFoundGPUDataCount) + " submesh(es) (Skeletal mesh : " + skMesh.GetName() + ") couldn't be found");
	
	invalidGPUDataCount = notFoundGPUDataCount = 0;

	for (SkeletalData& data : skMesh.skeletonDatas)
	{
		if (data.GPUData == nullptr)
		{
			invalidGPUDataCount++;
			return;
		}

		bool found = false;

		GPUSkeletalData const* gpuSkData = data.GPUData;
		auto gpuSkIt = std::find_if(GPUSkeletalDatas.begin(), GPUSkeletalDatas.end(),
			[gpuSkData](GPUSkeletalData const& gpuData) { return &gpuData == gpuSkData; });

		if (gpuSkIt != GPUSkeletalDatas.end())
		{
			found = true;

			// Free old space
			glNamedBufferSubData(skeletalDataVBO.GetID(), data.GPUData->offset, data.GPUData->size, nullptr);

			GPUSkeletalDatas.erase(gpuSkIt);
		}

		if (!found)
		{
			notFoundGPUDataCount++;
		}
	}

	if (invalidGPUDataCount)  Logger::Warning("RenderGPUWrapper - RemoveSkeletalMeshData : GPUSkeletalData(s) not valid in " + std::to_string(invalidGPUDataCount) + " submesh(es) (Skeletal mesh : " + skMesh.GetName() + ")");
	if (notFoundGPUDataCount) Logger::Warning("RenderGPUWrapper - RemoveSkeletalMeshData : GPUSkeletalData(s) from " + std::to_string(notFoundGPUDataCount) + " submesh(es) (Skeletal mesh : " + skMesh.GetName() + ") couldn't be found");
}



void RenderGPUWrapper::RemoveTextureData(Texture& text)
{
	if (text.GPUData == nullptr)
	{
		Logger::Warning("RenderGPUWrapper - RemoveTextureData : GPUData not valid : " + text.GetName());
		return;
	}

	// Find the texture GPU data
	GPUTextureData const* gpuTexData = text.GPUData;
	auto gpuTexIt = std::find_if(GPUTextureDatas.begin(), GPUTextureDatas.end(),
		[gpuTexData](GPUTextureData const& gpuData) { return &gpuData == gpuTexData; });

	if (gpuTexIt != GPUTextureDatas.end())
	{
		// Find the texture generated primitive
		GLTexture const* genTex = gpuTexIt->generatedTexture;
		auto genTexIt = std::find_if(GeneratedTextures.begin(), GeneratedTextures.end(),
			[genTex](GLTexture const& glTex) { return &glTex == genTex; });

		// Erase the generated texture, should always be true
		if (genTexIt != GeneratedTextures.end())
			GeneratedTextures.erase(genTexIt);

		// Erase the texture GPU data
		GPUTextureDatas.erase(gpuTexIt);

		return;
	}

	Logger::Error("RenderGPUWrapper - RemoveTextureData : GPUData not registered : " + text.GetName());
}

void RenderGPUWrapper::RemoveSkyboxData(Skybox& skybox)
{
	if (skybox.GPUData == nullptr)
	{
		Logger::Warning("RenderGPUWrapper - RemoveSkyboxData : GPUData not valid : " + skybox.GetName());
		return;
	}

	GLCubeMap const* gpuSkyData = skybox.GPUData;
	auto gpuSkyIt = std::find_if(GPUSkyboxDatas.begin(), GPUSkyboxDatas.end(),
		[gpuSkyData](GLCubeMap const& gpuData) { return &gpuData == gpuSkyData; });

	if (gpuSkyIt != GPUSkyboxDatas.end())
	{
		GPUSkyboxDatas.erase(gpuSkyIt);
		return;
	}

	Logger::Error("RenderGPUWrapper - RemoveTextureData : GPUData not registered : " + skybox.GetName());
}



void RenderGPUWrapper::RemoveTextureArray(Texture& text)
{
	GeneratedTextureArray.Remove(text);
}

#pragma endregion

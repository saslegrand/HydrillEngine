#pragma once

#include <vector>

#include "Renderer/RenderPipeline.hpp"
#include "Renderer/Primitives/VertexArray.hpp"
#include "Renderer/Primitives/VertexBuffer.hpp"
#include "Renderer/Primitives/ShaderProgram.hpp"
#include "Maths/Matrix4.hpp"
#include "Maths/Quaternion.hpp"
#include "Types/Color.hpp"

#include "EngineDLL.hpp"

class  ParticleComponent;
class  ParticleInstance;
class  ParticleEmitter;


/**
@brief Particle Render pipeline only use one shader and one mesh which is a quad.
2 Multidraws calls are used, (one for blended particles, one for opaques)
*/
class ParticleRenderPipeline : public RenderPipeline
{

private:

	struct ParticleInstanceGPU
	{
		alignas(4) float diffuseTextureLayer;
		alignas(4) float emissiveTextureLayer;
		alignas(4) float blendTexture01Layer;
		alignas(4) float blendTexture02Layer;
		alignas(4) float blendTexture03Layer;
		alignas(4) float blendTexture04Layer;

		alignas(4) float diffuseTextureWeight;
		alignas(4) float emissiveTextureWeight;
		alignas(4) float blendTexture01Weight;
		alignas(4) float blendTexture02Weight;
		alignas(4) float blendTexture03Weight;
		alignas(4) float blendTexture04Weight;


		alignas(16) Vector4 diffuseTextureUV;
		alignas(16) Vector4 emissiveTextureUV;
		alignas(16) Vector4 blendTextureUV_01;
		alignas(16) Vector4 blendTextureUV_02;
		alignas(16) Vector4 blendTextureUV_03;
		alignas(16) Vector4 blendTextureUV_04;

		alignas(16) Color4  color;
		alignas(16) Color3  emissiveColor;
		alignas(4)  float   emissiveStrength;
	};

	struct BlendParticleInstance
	{
		float distanceFromCamera = 0.0f;
		const ParticleInstance* particle = nullptr;
		bool  billboard = false;

		float diffuseTextureWeight;
		float emissiveTextureWeight;
		float blendTexture01Weight;
		float blendTexture02Weight;
		float blendTexture03Weight;
		float blendTexture04Weight;

		float  diffuseTextureLayer;
		float  emissiveTextureLayer;
		float  blendTexture01Layer;
		float  blendTexture02Layer;
		float  blendTexture03Layer;
		float  blendTexture04Layer;
	};

//	Variables

private:

	VertexArray  m_VAO;
	VertexBuffer m_VBO;
	VertexBuffer m_VBOParticleMatrices;
	unsigned int m_indirectBuffer = 0;
	unsigned int m_particlesSSBO = 0;

	std::vector<BlendParticleInstance> m_sortedBlendedInstances;
	std::vector<ParticleInstanceGPU> m_opaqueParticleInstanceGPU;
	std::vector<ParticleInstanceGPU> m_blendedParticleInstanceGPU;

	std::vector<Matrix4> m_opaqueParticleInstanceModels;
	std::vector<Matrix4> m_blendedParticleInstanceModels;

	ShaderProgram m_shader;
	std::vector<ParticleComponent*> m_particleComponents;

	Quaternion m_billboardRotation;
	Vector3 m_cameraForward;

//	Constructors & Destructors

public:

	ENGINE_API ParticleRenderPipeline();

//	Functions

private:

	/**
	@brief Set up multidraw command given and instance count

	@param instanceCount: quad instance number
	*/
	void SetupCommand(unsigned int instanceCount);

	/**
	@brief Generate particles instances from an emitter and send datas to an SSBO and model matrix in a VBO

	@param emitter: emitter to generate GPU instances from
	@param & particlesCount: particle count is modified inside the function
	*/
	void GenerateOpaqueInstancesFromEmitter(const ParticleEmitter& emitter);

	/**
	@brief Generate blended particles instances and send datas to an SSBO and model matrix in a VBO

	@param instances: List of sorted instances to generate
	*/
	void GenerateAllBlendedInstances(const std::vector<BlendParticleInstance>& instances);

	/**
	@brief Send texture datas to a GPU particle instance

	@param dst : particle instance to send textures to
	@param src : particle emitter to retrieve texture datas from
	*/
	void SendTextureDatas(ParticleInstanceGPU& dst, const ParticleEmitter& src);

	/**
	@brief Send instance datas to a GPU particle instance

	@param dst : particle instance to send instance datas to
	@param src : particle instance emitter to retrieve datas from
	*/
	void SendInstanceDatas(ParticleInstanceGPU& dst, const ParticleInstance& src);

public:

	/**
	@brief Initialiez shader, buffers and vertex array
	*/
	void Initialize() override;

	/**
	@brief Prerender function of the render pipeline
	*/
	void PreRender() override;

	/**
	@brief Clear all instances registered to this pipeline
	*/
	ENGINE_API void ClearParticleInstances();


	/**
	@brief Register a particles component instances to this pipeline

	@param particleComponent : Particle Component instance to add
	*/
	ENGINE_API void AddParticleInstance(ParticleComponent* particleComponent);


	/**
	@brief Unregister a particles component instances from this pipeline

	@param particleComponent : Particle Component instance to remove
	*/
	ENGINE_API void RemoveParticleInstance(ParticleComponent* particleComponent);


	/**
	@brief Render all registered mesh and skeletal mesh instance with fewer openGL calls
	*/
	ENGINE_API void Render();
};

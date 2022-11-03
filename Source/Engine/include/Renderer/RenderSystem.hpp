#pragma once

#include <queue>
#include <memory>

#include <Refureku/TypeInfo/Archetypes/Struct.h>

#include "Types/Color.hpp"
#include "Maths/Vector2.hpp"
#include "EngineDLL.hpp"

#include "Renderer/OpaqueRenderPipeline.hpp"
#include "Renderer/TransparentRenderPipeline.hpp"
#include "Renderer/SkyboxRenderPipeline.hpp"
#include "Renderer/ParticleRenderPipeline.hpp"
#include "Renderer/DebugRenderPipeline.hpp"
#include "Renderer/RenderGPUWrapper.hpp"
#include "Renderer/MeshUnlitShader.hpp"
#include "Renderer/MeshLitShader.hpp"
#include "Renderer/MeshPBRShader.hpp"
#include "Renderer/PostProcess.hpp"
#include "Renderer/RenderObjects/Camera.hpp"
#include "Renderer/RenderObjects/Light.hpp"
#include "Renderer/RenderpassParameters.hpp"
#include "Renderer/GraphicsSettings.hpp"
#include "Renderer/ShadowProcess.hpp"
#include "Renderer/RenderUtils.hpp"
#include "Tools/Event.hpp"

//	Forward declarations
class LightComponent;
class MeshComponent;
class SkeletalMeshComponent;
class GameObject;
class HYGUID;

struct MeshInstance;
struct SkeletalMeshInstance;

/**
@brief
*/
class RenderSystem
{
//	Variables

private:

	struct RenderInitializer
	{
		RenderInitializer();
	};

	unsigned int m_CameraUBO = 0;
	unsigned int m_LightUBO = 0;
	unsigned int m_settingUBO = 0;

	RenderInitializer		  m_initializer;
	OpaqueRenderPipeline	  m_opaqueRenderPipeline;
	TransparentRenderPipeline m_transparentRenderPipeline;
	SkyboxRenderPipeline	  m_skyboxRenderPipeline;
	ParticleRenderPipeline    m_particleRenderPipeline;

	ShadowProcess m_shadowProcess;

	std::vector<std::unique_ptr<LightComponent>> m_lights;

	std::vector<unsigned int>    m_renderPipelinesCallOrder;
	std::vector<RenderPipeline*> m_renderPipelines;

	std::vector<const MeshInstance*>		   m_allMeshInstances;
	std::vector<const SkeletalMeshInstance*>   m_allSkeletalMeshInstances;

public:

	RenderUtils utils;

	GraphicsSettings graphicsSettings;

	Color4	clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	RenderGPUWrapper     GPUWrapper;
	DebugRenderPipeline  debugRenderPipeline;

	MeshUnlitShader unlitShader;
	MeshLitShader	litShader;
	MeshPBRShader	pbrShader;

//	Constructors & Destructors

public:

	RenderSystem() = default;
	~RenderSystem();

//	Functions

private:

	/**
	@brief Update Uniform buffer object of lights
	*/
	void UpdateLightUBO();

public:

	/**
	@brief Initialize the render system and the default pipelines
	*/
	void Initialize();

	/**
	@brief Render calls of all renderer types of the current scene
	
	@param activeCamera : camera to render from
	*/
	ENGINE_API void Render(RenderpassParameters& param);

	/**
	@brief Attach a new skybox to the render system

	@param skybox : new skybox to attach
	*/
	ENGINE_API void AttachNewSkybox(Skybox* skybox);

	ENGINE_API Skybox* GetAttachedSkybox() const;

	/**
	@brief Register mesh instance to its correct pipeline

	@param instance : mesh instance to register
	*/
	void Register(MeshInstance* instance);

	/**
	@brief Unregister mesh instance from the renderer system

	@param instance : mesh instance to unregister
	*/
	void Unregister(MeshInstance* instance);

	/**
	@brief Register skeletal mesh instance to its correct pipeline

	@param instance : skeletal mesh instance to register
	*/
	void Register(SkeletalMeshInstance* instance);

	/**
	@brief Unregister skeletal mesh instance from the renderer system

	@param instance : skeletal mesh instance to unregister
	*/
	void Unregister(SkeletalMeshInstance* instance);
	
	/**
	@brief Register a particle component to the renderer system

	@param particleComp : particle component to register
	*/
	void Register(ParticleComponent* particleComp);

	/**
	@brief Unregister a particle component from the renderer system

	@param particleComp : particle component to unregister
	*/
	void Unregister(ParticleComponent* particleComp);

	/**
	@brief Add a new light to the system

	@param archetype : Archetype of the light
	@param owner : Owner of the light
	@param id : Unique identifier of the light

	@return LightComponent* : Created light, nullptr if not valid
	*/
	LightComponent* AddLightInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id);

	/**
	@brief Remove an existing light in the system

	@param light : light to remove
	*/
	void RemoveLightInstance(LightComponent& lightComp);

	/**
	@brief Get all mesh instances that were registered in the render system

	@return A vector of all mesh instances
	*/
	ENGINE_API const std::vector<const MeshInstance*>& GetAllMeshInstances() const;

	/**
	@brief Get all skeletal mesh instances that were registered in the render system

	@return A vector of all skeletal mesh instances
	*/
	ENGINE_API const std::vector<const SkeletalMeshInstance*>& GetAllSkeletalMeshInstances() const;

	/**
	@brief Get all skeletal lights that were registered in the render system

	@return A vector of all lights
	*/
	ENGINE_API const std::vector<std::unique_ptr<LightComponent>>& GetAllLights() const;

	ENGINE_API size_t GetPushedRenderPipelineCount() const;

	/**
	@brief Push a render pipeline that will be called during the render phase, you can chose the priority, the greater it is, the latest it will be called

	@param renderPipeline : the render pipeline to push
	@param priority : order in call;
	*/
	ENGINE_API void PushRenderPipeline(RenderPipeline* renderPipeline, unsigned int priority);

	/**
	@brief Remove the last render pipeline that was pushed in the render system
	
	@param count : number of pop to do
	*/
	ENGINE_API void PopRenderPipeline(unsigned int count = 1);

	/**
	@brief Send Camera to shader's camera Uniform buffer object

	@param Camera to send
	*/
	ENGINE_API void SendCameraUBO(const Camera& camera);

	/**
	@brief Send Lights to shader's light Uniform buffer object

	@param Light to send
	*/
	ENGINE_API void SendLightUBO(const std::vector<Light>& lights);
};

	#include "Renderer/RenderSystem.hpp"

#include <glad/gl.h>

#include "Core/Logger.hpp"
#include "Maths/Maths.hpp"
#include "Renderer/MeshInstance.hpp"
#include "Renderer/Shadertype.hpp"
#include "Renderer/MaterialSurface.hpp"
#include "Renderer/RenderConfig.hpp"
#include "ECS/MeshComponent.hpp"
#include "ECS/SkeletalMeshComponent.hpp"
#include "ECS/LightComponent.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "EngineContext.hpp"

//	DEBUG
void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	printf("OpenGL: %s\n", message);
}

RenderSystem::RenderInitializer::RenderInitializer()
{
	gladLoaderLoadGL();

	// Setup KHR debug
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(debugCallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
}

RenderSystem::~RenderSystem()
{
	glDeleteBuffers(1, &m_CameraUBO);
	glDeleteBuffers(1, &m_LightUBO);
}


void RenderSystem::Initialize()
{
	graphicsSettings.Initialize();

	//	ShadersPipeline
	unlitShader.Initialize();
	litShader.Initialize();
	pbrShader.Initialize();

	GPUWrapper.CreateVertexObjects();
	GPUWrapper.GeneratedTextureArray.Generate();
	GPUWrapper.GeneratedDepthMapTextureArray.Generate();
	GPUWrapper.GeneratedDepthCubeMapTextureArray.Generate();
	GPUWrapper.GeneratedCascadeDepthMapTextureArray.Generate();

	utils.Initialize();

	//	Pipelines
	m_opaqueRenderPipeline.Initialize();
	m_transparentRenderPipeline.Initialize();
	m_skyboxRenderPipeline.Initialize();
	debugRenderPipeline.Initialize();
	m_particleRenderPipeline.Initialize();

	m_shadowProcess.Initialize();

	GPUWrapper.GeneratedDepthMapTextureArray.BindUnit(10);
	GPUWrapper.GeneratedDepthCubeMapTextureArray.BindUnit(11);
	GPUWrapper.GeneratedCascadeDepthMapTextureArray.BindUnit(12);

	PushRenderPipeline(&m_opaqueRenderPipeline, 0);
	PushRenderPipeline(&m_skyboxRenderPipeline, 1);
	PushRenderPipeline(&m_particleRenderPipeline, 2);
	PushRenderPipeline(&m_transparentRenderPipeline, 3);
	PushRenderPipeline(&debugRenderPipeline, 4);

	glCreateBuffers(1, &m_CameraUBO);
	glNamedBufferData(m_CameraUBO, 2 * sizeof(Matrix4) + sizeof(Vector3), NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, UBOIndex_Camera, m_CameraUBO, 0, 2 * sizeof(Matrix4) + sizeof(Vector3));

	glCreateBuffers(1, &m_LightUBO);
	glNamedBufferData(m_LightUBO, sizeof(Light) * Max_Lights_Count + sizeof(int), NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, UBOIndex_Lights, m_LightUBO, 0, sizeof(Light) * Max_Lights_Count + sizeof(int));
	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RenderSystem::AttachNewSkybox(Skybox* skybox)
{
	m_skyboxRenderPipeline.attachedSkybox = skybox;
}

Skybox* RenderSystem::GetAttachedSkybox() const
{
	return m_skyboxRenderPipeline.attachedSkybox;
}

void RenderSystem::Register(MeshInstance* instance)
{
	if (instance->material)
	{
		switch (instance->material->surface)
		{
		case MaterialSurface::OPAQUE:
			m_opaqueRenderPipeline.AddMeshInstance(instance);
			break;
		case MaterialSurface::TRANSLUCENT:
			m_transparentRenderPipeline.AddMeshInstance(instance);
			break;
		default: break;
		}
	}
	else
	{
		//	Opaque pipeline by default (without materials)
		m_opaqueRenderPipeline.AddMeshInstance(instance);
	}

	auto it = std::find(m_allMeshInstances.begin(), m_allMeshInstances.end(), const_cast<MeshInstance*>(instance));
	if (it == m_allMeshInstances.end())
	{
		m_allMeshInstances.emplace_back(instance);
	}
}

void RenderSystem::Unregister(MeshInstance* instance)
{
	if (instance->savedPipeline) instance->savedPipeline->RemoveMeshInstance(instance);

	auto it = std::find(m_allMeshInstances.begin(), m_allMeshInstances.end(), const_cast<MeshInstance*>(instance));
	if (it != m_allMeshInstances.end())
	{
		m_allMeshInstances.erase(it);
	}
}

void RenderSystem::Register(SkeletalMeshInstance* instance)
{
	if (instance->material)
	{
		switch (instance->material->surface)
		{
		case MaterialSurface::OPAQUE:
			m_opaqueRenderPipeline.AddSkeletalMeshInstance(instance);
			break;
		case MaterialSurface::TRANSLUCENT:
			m_transparentRenderPipeline.AddSkeletalMeshInstance(instance);
			break;
		default: break;
		}
	}
	else
	{
		//	Opaque pipeline by default (without materials)
		m_opaqueRenderPipeline.AddSkeletalMeshInstance(instance);
	}

	auto it = std::find(m_allSkeletalMeshInstances.begin(), m_allSkeletalMeshInstances.end(), const_cast<SkeletalMeshInstance*>(instance));
	if (it == m_allSkeletalMeshInstances.end())
	{
		m_allSkeletalMeshInstances.emplace_back(instance);
	}
}

void RenderSystem::Unregister(SkeletalMeshInstance* instance)
{
	if (instance->savedPipeline) instance->savedPipeline->RemoveSkeletalMeshInstance(instance);
	
	auto it = std::find(m_allSkeletalMeshInstances.begin(), m_allSkeletalMeshInstances.end(), const_cast<SkeletalMeshInstance*>(instance));
	if (it != m_allSkeletalMeshInstances.end())
	{
		m_allSkeletalMeshInstances.erase(it);
	}
}


void RenderSystem::Register(ParticleComponent* particleComp)
{
	m_particleRenderPipeline.AddParticleInstance(particleComp);
}

void RenderSystem::Unregister(ParticleComponent* particleComp)
{
	m_particleRenderPipeline.RemoveParticleInstance(particleComp);
}


LightComponent* RenderSystem::AddLightInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id)
{
	LightComponent& light = *m_lights.emplace_back(archetype.makeUniqueInstance<LightComponent>(owner, id));
	return &light;
}

void RenderSystem::RemoveLightInstance(LightComponent& lightComp)
{
	// Find the the camera
	auto it = std::find_if(m_lights.begin(), m_lights.end(),
		[&lightComp](std::unique_ptr<LightComponent>& _lightComp) { return _lightComp.get() == &lightComp; });

	if (it == m_lights.end())
	{
		// Should not happer
		Logger::Error("RenderSystem - The light component you tried to remove not exists");
		return;
	}

	// Remove the camera
	m_lights.erase(it);
}

const std::vector<const MeshInstance*>& RenderSystem::GetAllMeshInstances() const
{
	return m_allMeshInstances;
}

const std::vector<const SkeletalMeshInstance*>& RenderSystem::GetAllSkeletalMeshInstances() const
{
	return m_allSkeletalMeshInstances;
}

const std::vector<std::unique_ptr<LightComponent>>& RenderSystem::GetAllLights() const
{
	return m_lights;
}

size_t RenderSystem::GetPushedRenderPipelineCount() const
{
	return m_renderPipelines.size();
}

void RenderSystem::PushRenderPipeline(RenderPipeline* renderPipeline, unsigned int priority)
{
	unsigned int callIndex = Maths::Clamp(priority, static_cast<unsigned int>(0), static_cast<unsigned int>(m_renderPipelinesCallOrder.size()));

	m_renderPipelinesCallOrder.insert(m_renderPipelinesCallOrder.begin() + callIndex, static_cast<unsigned int>(m_renderPipelines.size()));
	m_renderPipelines.push_back(renderPipeline);
}

void RenderSystem::PopRenderPipeline(unsigned int count)
{
	for (unsigned int i = 0; i < count; i++)
	{
		if (m_renderPipelines.empty()) break;

		size_t index = m_renderPipelines.size() - 1;
		auto it = std::find(m_renderPipelinesCallOrder.begin(), m_renderPipelinesCallOrder.end(), index);

		if (it != m_renderPipelinesCallOrder.end())
		{
			m_renderPipelinesCallOrder.erase(it);
		}

		m_renderPipelines.pop_back();
	}
}

void RenderSystem::Render(RenderpassParameters& param)
{
	Camera* currentCam = param.GetBindedCamera();
	if (!currentCam) return;

	SystemManager::GetCameraSystem().renderingCamera = currentCam;

	//	Pre render pass

	SendCameraUBO(*currentCam);
	UpdateLightUBO();

	glEnable(GL_DEPTH_TEST);

	for (unsigned int i : m_renderPipelinesCallOrder)
	{
		m_renderPipelines[i]->PreRender();
	}

	if(graphicsSettings.GPUSettings.shadows) m_shadowProcess.ProcessShadowMaps();

	param.BeginCapture();

	//	Render pass

	glEnable(GL_CULL_FACE);

	Vector2 viewportDimensions = param.GetViewportDimensions();
	glViewport(0, 0, static_cast<GLsizei>(viewportDimensions.x), static_cast<GLsizei>(viewportDimensions.y));
	glClearColor(Maths::Pow(clearColor.r,2.2f), Maths::Pow(clearColor.g, 2.2f), Maths::Pow(clearColor.b, 2.2f), clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	for (unsigned int i : m_renderPipelinesCallOrder)
	{
		m_renderPipelines[i]->Render();
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	param.EndCapture();

	SystemManager::GetCameraSystem().renderingCamera = nullptr;
}

void RenderSystem::UpdateLightUBO()
{
	GLsizei lightsSize = 0;

	std::vector<Light> gpuLights;

	int count = 0;
	for (auto& light : m_lights)
	{
		if (count >= 8)
		{
			break;
		}

		gpuLights.emplace_back(
			light->gameObject.transform.Position(),
			light->gameObject.transform.Forward(),
			light->diffuse,
			light->ambient,
			light->specular,
			Vector2(light->innerCutoff, light->outerCutoff),
			light->radius,
			light->intensity,
			light->type,
			light->castShadows,
			light->enabled
		);

		count++;
	}

	SendLightUBO(gpuLights);
}

void RenderSystem::SendCameraUBO(const Camera& camera)
{
	//	Send Camera to uniform buffer
	glNamedBufferSubData(m_CameraUBO, 0, sizeof(Matrix4), camera.GetProjectionMatrix().elements);

	glNamedBufferSubData(m_CameraUBO, sizeof(Matrix4), sizeof(Matrix4), camera.GetViewMatrix().elements);

	glNamedBufferSubData(m_CameraUBO, 2 * sizeof(Matrix4), sizeof(Vector3), camera.GetPosition().elements);
}


void RenderSystem::SendLightUBO(const std::vector<Light>& lights)
{
	size_t count = lights.size();

	//	Send lights
	glNamedBufferSubData(m_LightUBO, 0, sizeof(Light) * count, lights.data());

	//	Send lights count
	glNamedBufferSubData(m_LightUBO, sizeof(Light) * Max_Lights_Count, sizeof(int), &count);
}

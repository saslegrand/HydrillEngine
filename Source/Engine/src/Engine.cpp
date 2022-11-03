#include "Engine.hpp"

#include <iostream>

#include <Refureku/Refureku.h>

#include "Resources/Resource/Skybox.hpp"
#include "ECS/MeshComponent.hpp"
#include "ECS/CameraComponent.hpp"
#include "ECS/LightComponent.hpp"
#include "ECS/Behavior.hpp"

#include "Tools/PathConfig.hpp"

Engine::Engine(class Window* window)
	: m_logger("EngineLog"), m_inputSystem(window)
{
	// Create game context
	GameContext::m_instance = &m_gameContext;
	GameContext::m_instance->m_engine = this;

	// Create Engine context
	EngineContext::m_instance = &m_context;
	EngineContext::Instance().taskQueue			= &m_taskQueue;
	EngineContext::Instance().logger			= &m_logger;
	EngineContext::Instance().time				= &m_time;
	EngineContext::Instance().resourcesManager	= &m_resourcesManager;
	EngineContext::Instance().sceneManager		= &m_sceneManager;
	EngineContext::Instance().inputSystem    	= &m_inputSystem;
	EngineContext::Instance().systemManager   	= &m_systemManager;
}

Engine::~Engine()
{
}


void Engine::Initialize()
{
	Logger::Info("Initializing Engine");

	SystemManager::GetRenderSystem().Initialize();

	// INITIALIZE MANAGERS
	m_resourcesManager.Initialize();
	m_sceneManager.Initialize();

	m_systemManager.Initialize();
}

void Engine::Shutdown()
{
	Logger::Info("Shutting down Engine");
}

void Engine::FrameStart()
{
	m_time.Update();
	m_inputSystem.Update(m_time.GetUnscaledDeltaTime());

	m_systemManager.FrameStart();
}

void Engine::FrameEnd()
{
	m_inputSystem.Refresh();

	m_taskQueue.InvokeTasks();
}

void Engine::GameInitialize()
{
	m_gameRenderParameters.Initialize();
	m_gameRenderParameters.EnablePostProcess();
}

void Engine::GameStart()
{
	EngineContext::Instance().playing = true;

	m_systemManager.Start();
}

void Engine::GameStop()
{
	EngineContext::Instance().playing = false;

	m_systemManager.Stop();
}

void Engine::Update()
{
	m_sceneManager.UpdateGameObjectTransforms();
	m_systemManager.PermanentUpdate();
}

void Engine::GameUpdate()
{
	// Update
	// ======
	m_systemManager.Update(m_time);
}

void Engine::GameLateUpdate()
{
	m_systemManager.LateUpdate();
}

void Engine::GameRender()
{
	m_gameRenderParameters.UpdateFrameBuffer();
	
	//	Here call render with scene settings
	m_gameRenderParameters.BindCamera(SystemManager::GetCameraSystem().GetActiveCamera());
	SystemManager::GetRenderSystem().Render(m_gameRenderParameters);

	m_gameRenderParameters.DrawImage();
}

void Engine::SetGameWindowSize(Vector2 dim)
{
	m_gameRenderParameters.SetContainerDimensions(dim);
}
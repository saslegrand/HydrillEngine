#pragma once

#include "EngineDLL.hpp"
#include "EngineContext.hpp"
#include "GameContext.hpp"
#include "Core/Time.hpp"
#include "Core/Logger.hpp"
#include "Core/TaskQueue.hpp"
#include "IO/InputSystem.hpp"
#include "ECS/Systems/BehaviorSystem.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/ParticleSystem.hpp"
#include "ECS/Systems/MeshSystem.hpp"
#include "ECS/Systems/SoundSystem.hpp"
#include "ECS/Systems/SystemManager.hpp"
#include "Resources/ResourcesManager.hpp"
#include "Resources/SceneManager.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Renderer/RenderObjects/Camera.hpp"
#include "Physics/PhysicsSystem.hpp"

/**
Main class of the engine
*/
class Engine
{
//	Constructors & Destructors

public:

	/**
	@brief Engine creation need window to initialize inputs
	*/
	ENGINE_API Engine(class Window* window);
	ENGINE_API virtual ~Engine();

//	Variables

private:

	EngineContext		m_context;
	GameContext			m_gameContext;

	Time				m_time;
	Logger				m_logger;
	TaskQueue			m_taskQueue;
	InputSystem			m_inputSystem;
	SceneManager		m_sceneManager;
	ResourcesManager	m_resourcesManager;
	SystemManager		m_systemManager;
	
	RenderpassParameters m_gameRenderParameters;
	Camera m_camera;


//	Functions

public:

	/**
	@brief Call a the start of the loop application (systems update)
	*/
	ENGINE_API void FrameStart();

	/**
	@brief Call a the end of the loop application (systems end update)
	*/
	ENGINE_API void FrameEnd();

	/**
	@brief Update function is called once each frame
	*/
	ENGINE_API void Update();

	/**
	@brief Game Initialization function
	*/
	ENGINE_API void GameInitialize();

	/**
	@brief Game start function
	*/
	ENGINE_API void GameStart();

	/**
	@brief Game stop function
	*/
	ENGINE_API void GameStop();


	/**
	@brief Game update function is called once each frame
	*/
	ENGINE_API void GameUpdate();

	/**
	@brief Game late update function is called once each frame
	*/
	ENGINE_API void GameLateUpdate();

	/**
	@brief Game render function
	*/
	ENGINE_API void GameRender();


	/**
	@brief Game window size

	@param dim : new game window dimensions
	*/
	ENGINE_API void SetGameWindowSize(Vector2 dim);

	/*
	@brief Initialize engine
	*/
	ENGINE_API void Initialize();

	/*
	@brief Shutdown engine
	*/
	ENGINE_API void Shutdown();

};

#pragma once

#include "EngineDLL.hpp"

/**
@brief This class allow access to differents engine systems/managers from anywhere
*/
struct EngineContext
{
	friend class Engine;

private:

	static EngineContext* m_instance;

public:

	class Time*				time = nullptr;
	class Logger*			logger = nullptr;
	class TaskQueue*		taskQueue = nullptr;
	class ResourcesManager*	resourcesManager = nullptr;
	class SceneManager*		sceneManager = nullptr;
	class InputSystem*		inputSystem = nullptr;
	class SystemManager*   systemManager = nullptr;

	bool playing = false;
public:

	static ENGINE_API EngineContext& Instance();
};
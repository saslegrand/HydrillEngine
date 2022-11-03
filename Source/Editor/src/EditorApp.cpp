#include <memory>

#include "EditorApp.hpp"

//	DEV -> Remove later
#include <imgui/imgui.h>

#include <Engine.hpp>
#include <DllWrapper.hpp>
#include <ECS/CameraComponent.hpp>

#include "Editor.hpp"
#include "EditorContext.hpp"
#include "EditorSelectionManager.hpp"
#include "Windows/WindowManager.hpp"
#include "io/Window.hpp"
#include "io/InputSystem.hpp"

//	Constructor and destructor are defined here 
//  for fast forward class declaration with unique ptr

EditorApp::EditorApp() {}

EditorApp::~EditorApp() { }

#include "Tools/GameBuilder.hpp"
void EditorApp::Initialize(const char* windowName)
{
	//	First create window, this is the first step to do in executing this program, 
	//	It contains GLFW instances and OpenGL initializations

	m_window = std::make_unique<Window>(windowName);

	//	Then create editor and engine by passing window to them
	//	-------------------------------------------------------

	//	Window is needed for input system
	m_engine = std::make_unique<Engine>(m_window.get());

	m_scriptingWrapper = std::make_unique<DllWrapper>(BINARIES_ROOT + std::string(R"(Scripting.dll)"));

	//	Window id needed for ImGui
	m_editor = std::make_unique<Editor>(m_window.get());

	EditorContext& context = EditorContext::Instance();
	EditorContext::Instance().scriptingWapper = m_scriptingWrapper.get();

	//	Initializations
	//	---------------

	m_engine->Initialize();
	m_editor->Initialize();

	//	Set callbacks

	resizeEventID = m_window->resizeEvent.AddCallback(&EditorApp::ResizeUpdate, *this);
	moveEventID   = m_window->moveEvent.AddCallback(&EditorApp::ResizeUpdate, *this);

	playGameCallbackID = EditorContext::Instance().onEnginePlayed.AddCallback(&EditorApp::PlayGame, *this);
	pauseGameCallbackID = EditorContext::Instance().onEnginePaused.AddCallback(&EditorApp::PauseGame, *this);
	stopGameCallbackID = EditorContext::Instance().onEngineStopped.AddCallback(&EditorApp::StopGame, *this);
}

void EditorApp::Loop()
{
	while (m_window->ShouldWindowLoop())
	{
		FrameStart();
		
		Update();

		Render();

		FrameEnd();
	}
}

void EditorApp::CloseApp()
{
	m_window->CloseWindow();
}

void EditorApp::Shutdown()
{
	EditorContext::Instance().onEnginePlayed.RemoveCallback(playGameCallbackID);
	EditorContext::Instance().onEnginePaused.RemoveCallback(pauseGameCallbackID);
	EditorContext::Instance().onEngineStopped.RemoveCallback(stopGameCallbackID);

	m_window->resizeEvent.RemoveCallback(resizeEventID);
	m_window->moveEvent.RemoveCallback(moveEventID);
	
	m_engine->Shutdown();
}

void EditorApp::FrameStart()
{
	m_window->FrameStart();
	m_engine->FrameStart();
	m_editor->FrameStart();
}


void EditorApp::FrameEnd()
{
	m_engine->FrameEnd();
	m_window->FrameEnd();

	if (m_editor->shutdown)
		CloseApp();
}


void EditorApp::Update()
{
	GameState& state = EditorContext::Instance().gameState;

	if (state == GameState::Play)
		m_engine->GameUpdate();

	if (!m_window->minimized) m_editor->Update();

	if (state == GameState::Play)
		m_engine->GameLateUpdate();

	m_engine->Update();

	m_editor->Display();
}


void EditorApp::Render()
{
	m_editor->Render();
}


void EditorApp::ResizeUpdate()
{
	FrameStart();

	Update();

	Render();

	FrameEnd();
}


void EditorApp::PlayGame()
{
	EngineContext& engineContext = EngineContext::Instance();
	EditorContext& editorContext = EditorContext::Instance();

	GameState& state = editorContext.gameState;
	if (state == GameState::Play) return;

	if (state == GameState::Sleep)
	{
		// Save cam pos + last scene opened
		editorContext.windowManager->SaveProjectSettings();

		// Save the scene, when game is stoped, retrieve previous scene state
		engineContext.sceneManager->SaveCurrentScene();

		//m_engine->GameStop();
		m_engine->GameStart();

		// Save the active camera
		CameraComponent* camComp = SystemManager::GetCameraSystem().GetActiveCameraComponent();
		if (camComp != nullptr)
			editorContext.idLastActiveCamGO = camComp->gameObject.GetUID();
	}

	engineContext.time->SetTimeScale(savedTimeScale);
	state = GameState::Play;
}


void EditorApp::PauseGame()
{
	GameState& state = EditorContext::Instance().gameState;
	if (state == GameState::Pause || state == GameState::Sleep) return;

	EngineContext& engineContext = EngineContext::Instance();

	savedTimeScale = engineContext.time->GetTimeScale();
	engineContext.time->SetTimeScale(0.0f);

	state = GameState::Pause;
}

void EditorApp::StopGame()
{
	EngineContext& engineContext = EngineContext::Instance();
	EditorContext& editorContext = EditorContext::Instance();

	GameState& state = editorContext.gameState;
	if (state == GameState::Sleep) return;

	m_engine->GameStop();

	// Load previous cam pos + last scene opened
	editorContext.windowManager->LoadProjectSettings();

	engineContext.time->SetTimeScale(0.0f);

	Scene* curScene = engineContext.sceneManager->GetCurrentScene();
	if (GameObject* goCam = curScene->GetGameObject(editorContext.idLastActiveCamGO))
		CameraSystem::SetActiveCamera(*goCam->GetComponent<CameraComponent>());

	state = GameState::Sleep;
}

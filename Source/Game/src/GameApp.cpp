#include <memory>

#include "GameApp.hpp"

//	DEV -> Remove later
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <Engine.hpp>
#include <Tools/PathConfig.hpp>
#include <DllWrapper.hpp>

#include "GameContext.hpp"
#include "io/Window.hpp"
#include "io/InputSystem.hpp"

//	Constructor and destructor are defined here 
//  for fast forward class declaration with unique ptr

GameApp::GameApp() 
{}

GameApp::~GameApp() {}

void GameApp::Initialize(const char* windowName)
{
	//	First create window, this is the first step to do in executing this program, 
	//	It contains GLFW instances and OpenGL initializations

	m_window = std::make_unique<Window>(windowName);

	m_window->FrameStart();
	m_window->FrameEnd();

	//	Then create editor and engine by passing window to them
	//	-------------------------------------------------------

	//	Window is needed for input system
	m_engine = std::make_unique<Engine>(m_window.get());

	// Load scripting dll
	m_scriptingWrapper = std::make_unique<DllWrapper>(BINARIES_ROOT + std::string(R"(Scripting.dll)"));

	//	Initializations
	//	---------------

	m_engine->Initialize();
	m_engine->GameInitialize();

	m_engine->SetGameWindowSize(Vector2(static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight())));

	//	Set callbacks
	resizeEventID = m_window->resizeEvent.AddCallback(&GameApp::ResizeUpdate, *this);
	moveEventID = m_window->moveEvent.AddCallback(&GameApp::ResizeUpdate, *this);
	
	// Load game settings and start initial scene
	if (!GameContext::LoadSettings())
		CloseApp();
}

void GameApp::FrameStart()
{
	Input::Mouse::SetMode(MouseMode::Disabled);
	m_window->FrameStart();
	m_engine->FrameStart();
}

void GameApp::FrameEnd()
{
	m_engine->FrameEnd();
	m_window->FrameEnd();
}

#include <Maths/Vector2.hpp>
void GameApp::Update()
{
	/*static int frameCount = 0;
	const float frameMax = 600.f;
	static float frameSum = 0.f;
	static Vector2 minMaxFrame = Vector2(FLT_MAX, -FLT_MAX);

	float curFps = EngineContext::Instance().time->GetFPS();
	frameSum += curFps;

	if (curFps > minMaxFrame.y)
		minMaxFrame.y = curFps;

	if (curFps < minMaxFrame.x)
		minMaxFrame.x = curFps;

	frameCount++;
	if (frameCount == frameMax)
	{
		Logger::Info("FPS : " + std::to_string(frameSum / frameMax));
		Logger::Info("Min : " + std::to_string(minMaxFrame.x) + " | Max : " + std::to_string(minMaxFrame.y) + "\n");
		frameCount = 0;
		frameSum = 0.f;
		minMaxFrame = Vector2(FLT_MAX, -FLT_MAX);
	}*/

	m_engine->GameUpdate();
	m_engine->GameLateUpdate();
	m_engine->Update();
}

void GameApp::Render()
{
	m_engine->GameRender();
}

void GameApp::Loop()
{
	Logger::Info("start the game");

	EngineContext::Instance().sceneManager->UpdateGameObjectTransforms();

	GameContext::Start();
	FrameStart();

	while (m_window->ShouldWindowLoop())
	{
		FrameStart();

		Update();

		Render();

		FrameEnd();
	}

	GameContext::Stop();
}

void GameApp::CloseApp()
{
	m_window->CloseWindow();
}

void GameApp::Shutdown()
{
	m_engine->Shutdown();

	m_window->resizeEvent.RemoveCallback(resizeEventID);
	m_window->moveEvent.RemoveCallback(moveEventID);
}

void GameApp::ResizeUpdate()
{
	m_window->FrameStart();

	Render();

	m_window->FrameEnd();

	m_engine->SetGameWindowSize(Vector2(static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight())));
}

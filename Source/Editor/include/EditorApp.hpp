#pragma once

#include <BaseApp.hpp>

#include "Tools/Event.hpp"

/**
@brief Editor application
*/
class EditorApp : public BaseApp
{

//	Variables

private:

	std::unique_ptr<class Editor> m_editor = nullptr;
	
	bool m_close = false;

	float savedTimeScale = 1.f;

	CallbackID playGameCallbackID;
	CallbackID pauseGameCallbackID;
	CallbackID stopGameCallbackID;

//	Constructors & Destructors

public:

	EditorApp();
	virtual ~EditorApp();

//	Functions

private:

	/**
	@brief Close the application
	*/
	void CloseApp();

	/**
	@brief To place at the beginning of each frame
	*/
	void FrameStart() override;

	/**
	@brief To place at the end of each frame
	*/
	void FrameEnd() override;

	/**
	@brief Update phase of the application
	*/
	void Update() override;

	/**
	@brief Render phase of the application
	*/
	void Render() override;

	/**
	@brief Render loop without engine update, used by windows callbacks
	*/
	void ResizeUpdate() override;


	/**
	@brief Change game state to play
	*/
	void PlayGame();

	/**
	@brief Change game state to pause
	*/
	void PauseGame();

	/**
	@brief Change game state to sleep
	*/
	void StopGame();


public:

	/**
	@brief Initialize the editor application

	@param windowName : name of the window
	*/
	void Initialize(const char* windowName = "Hydrill Engine") override;

	/**
	@brief Main loop, it contains the update and rendering phases
	*/
	void Loop() override;

	/**
	@brief Shutdown the application
	*/
	void Shutdown() override;
};
#pragma once

#include <memory>

#include "io/Keyboard.hpp"
#include "io/Mouse.hpp"

/**
* This class manage Keyboard and Mouse systems
*/
class InputSystem
{
friend class Engine;
//friend class GameApp;

//	Constructors & Destructors

public:

	/**
	@brief Initialize Mouse and Keyboard systems given a window

	@param window : current window used to initialize Keyboard and Mouse
	*/
	InputSystem(class Window* window);

//	Functions

private:

	/**
	@brief Refresh inputs states.
	*/
	ENGINE_API void Refresh();

	/**
	@brief Update inputSystem
	*/
	ENGINE_API void Update(float unscaledDeltaTime);

	Input::Mouse    mouse;
	Input::Keyboard keyboard;

};

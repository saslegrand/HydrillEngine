#include "io/InputSystem.hpp"

#include "Generated/Inputs.rfks.h"

InputSystem::InputSystem(Window* window) : keyboard(window), mouse(window) {}

void InputSystem::Refresh()
{
	mouse.Refresh();
	keyboard.Refresh();
}


void InputSystem::Update(float unscaledDeltaTime)
{
	mouse.Update(unscaledDeltaTime);
}
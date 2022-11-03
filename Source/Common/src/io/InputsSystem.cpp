#include "io/InputsSystem.hpp"


InputsSystem::InputsSystem(Window* window) : keyboard(window), mouse(window) {}

void InputsSystem::Refresh()
{
	mouse.RefreshQueue();
	keyboard.RefreshQueue();
}
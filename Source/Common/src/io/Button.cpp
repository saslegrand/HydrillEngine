#include <io/Button.hpp>
#include <io/Inputs.hpp>

void Input::Button::Update(const int action)
{
	if (action == static_cast<int>(InputAction::Pressed))
	{
		pressed = down = true;
		up = false;
	}
	else if (action == static_cast<int>(InputAction::Unpressed))
	{
		unpressed = up = true;
		down = false;	
	}
}
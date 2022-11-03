#include <io/Button.hpp>

void Input::Button::Update(const int action)
{
	//	Is pressed in this frame?
	if (action == static_cast<int>(InputAction::Pressed))
	{
		pressed = down = true;
		up = false;
	}

	//	Is unpressed in this frame ?
	else if (action == static_cast<int>(InputAction::Unpressed))
	{
		unpressed = up = true;
		down = false;
	}
}
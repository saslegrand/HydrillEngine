#include "IO/InputAxis.hpp"


#include "Io/Keyboard.hpp"
#include "IO/Button.hpp"
#include "Maths/Maths.hpp"

using namespace Input;

Axis::Axis(int negId, int posId)
	: buttonIdNeg(negId), buttonIdPos(posId)
{
}

void Axis::Compute(float deltaTime)
{
	// Set raw value to null
	rawValue = 0.f;

	// Not valid
	if (buttonIdNeg < 0 || buttonIdPos < 0)
	{
		value = 0.f;
		return;
	}

	Button& negativeButton = Keyboard::GetKey(static_cast<Key>(buttonIdNeg));
	Button& positiveButton = Keyboard::GetKey(static_cast<Key>(buttonIdPos));

	// Two button pressed at the same time, stop motion
	if (negativeButton.down && positiveButton.down)
		return;

	// Set raw value signed
	if (negativeButton.down)
		rawValue = -1.f;
	else if (positiveButton.down)
		rawValue = 1.f;

	if (rawValue)
	{
		// One button pressed

		float lerpTime = timeToLerp * timeMultiplier;
		float sign = Maths::Sign(rawValue);

		elapsedTimeChanged += deltaTime * sign;
		elapsedTimeChanged = Maths::Clamp(elapsedTimeChanged, -lerpTime, lerpTime);

		float percentTime = elapsedTimeChanged / lerpTime;
		float percentLerp = (percentTime + 1.f) * 0.5f;

		value = Maths::Lerp(-amplitude, amplitude, percentLerp);
		rawValue *= amplitude;

		return;
	}

	// No button pressed

	if (value == 0.f)
		return;

	float sign = Maths::Sign(elapsedTimeChanged);
	elapsedTimeChanged -= deltaTime * sign;

	// Time has exceed 0
	if (sign != Maths::Sign(elapsedTimeChanged))
	{
		value = 0.f;
		return;
	}

	float lerpTime = timeToLerp * timeMultiplier;
	float percentTime = Maths::Abs(elapsedTimeChanged) / lerpTime;
	float percentLerp = 1.f - percentTime;

	value = Maths::Lerp(amplitude * sign, 0.f, percentLerp);
}
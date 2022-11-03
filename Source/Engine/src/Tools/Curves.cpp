#include "Maths/Maths.hpp"

#include "Generated/Curves.rfks.h"

std::array<float(*)(float), 30> EasingCurveFunctions::ComputeCurve
{ 
	ComputeEaseInSine,		ComputeEaseOutSine,		ComputeEaseInOutSine,
	ComputeEaseInQuad,		ComputeEaseOutQuad,		ComputeEaseInOutQuad,
	ComputeEaseInCubic,		ComputeEaseOutCubic,	ComputeEaseInOutCubic,
	ComputeEaseInQuart,		ComputeEaseOutQuart,	ComputeEaseInOutQuart,
	ComputeEaseInQuint,		ComputeEaseOutQuint,	ComputeEaseInOutQuint,
	ComputeEaseInExpo,		ComputeEaseOutExpo,		ComputeEaseInOutExpo,
	ComputeEaseInCirc,		ComputeEaseOutCirc,		ComputeEaseInOutCirc,
	ComputeEaseInBack,		ComputeEaseOutBack,		ComputeEaseInOutBack,
	ComputeEaseInElastic,	ComputeEaseOutElastic,	ComputeEaseInOutElastic,
	ComputeEaseInBounce,	ComputeEaseOutBounce,	ComputeEaseInOutBounce
};

float EasingCurveFunctions::ComputeEaseInSine(float x)
{
	return 1.f - Maths::Cos((x * Maths::PI) * 0.5f);
}


float EasingCurveFunctions::ComputeEaseOutSine(float x)
{
	return Maths::Sin((x * Maths::PI) * 0.5f);
}


float EasingCurveFunctions::ComputeEaseInOutSine(float x)
{
	return -(Maths::Cos(Maths::PI * x) - 1.f) * 0.5f;
}


float EasingCurveFunctions::ComputeEaseInQuad(float x)
{
	return x * x;
}


float EasingCurveFunctions::ComputeEaseOutQuad(float x)
{
	return 1.f - (1.f - x) * (1.f - x);
}


float EasingCurveFunctions::ComputeEaseInOutQuad(float x)
{
	return x < 0.5f ? 2.f * x * x : 1.f - Maths::Pow(-2.f * x + 2.f, 2.f) * 0.5f;
}


float EasingCurveFunctions::ComputeEaseInCubic(float x)
{
	return x * x * x;
}


float EasingCurveFunctions::ComputeEaseOutCubic(float x)
{
	return 1.f - Maths::Pow(1.f - x, 3.f);
}


float EasingCurveFunctions::ComputeEaseInOutCubic(float x)
{
	return x < 0.5 ? 4.f * x * x * x : 1.f - Maths::Pow(-2.f * x + 2.f, 3.f)  * 0.5f;
}


float EasingCurveFunctions::ComputeEaseInQuart(float x)
{
	return x * x * x * x;
}


float EasingCurveFunctions::ComputeEaseOutQuart(float x)
{
	return 1.f - Maths::Pow(1.f - x, 4.f);
}


float EasingCurveFunctions::ComputeEaseInOutQuart(float x)
{
	return x < 0.5 ? 8.f * x * x * x * x : 1.f - Maths::Pow(-2.f * x + 2.f, 4.f) * 0.5f;
}


float EasingCurveFunctions::ComputeEaseInQuint(float x)
{
	return x * x * x * x * x;
}

float EasingCurveFunctions::ComputeEaseOutQuint(float x)
{
	return 1.f - Maths::Pow(1.f - x, 5.f);
}


float EasingCurveFunctions::ComputeEaseInOutQuint(float x)
{
	return x < 0.5f ? 16.f * x * x * x * x * x : 1.f - Maths::Pow(-2.f * x + 2.f, 5.f) * 0.5f;
}


float EasingCurveFunctions::ComputeEaseInExpo(float x)
{
	return x == 0.f ? 0.f : Maths::Pow(2.f, 10.f * x - 10.f);
}


float EasingCurveFunctions::ComputeEaseOutExpo(float x)
{
	return x == 1.f ? 1.f : 1.f - Maths::Pow(2.f, -10.f * x);
}


float EasingCurveFunctions::ComputeEaseInOutExpo(float x)
{
	return x == 0.f
		? 0.f
		: x == 1.f
		? 1.f
		: x < 0.5f ? Maths::Pow(2.f, 20.f * x - 10.f) * 0.5f
		: (2.f - Maths::Pow(2.f, -20.f * x + 10.f)) * 0.5f;
}


float EasingCurveFunctions::ComputeEaseInCirc(float x)
{
	return 1.f - Maths::Sqrt(1.f - Maths::Pow(x, 2.f));
}


float EasingCurveFunctions::ComputeEaseOutCirc(float x)
{
	return Maths::Sqrt(1.f - Maths::Pow(x - 1.f, 2.f));
}


float EasingCurveFunctions::ComputeEaseInOutCirc(float x)
{
	return x < 0.5f
		? (1.f - Maths::Sqrt(1.f - Maths::Pow(2.f * x, 2.f))) * 0.5f
		: (Maths::Sqrt(1.f - Maths::Pow(-2.f * x + 2.f, 2.f)) + 1.f) * 0.5f;
}


float EasingCurveFunctions::ComputeEaseInBack(float x)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.f;

	return c3 * x * x * x - c1 * x * x;
}


float EasingCurveFunctions::ComputeEaseOutBack(float x)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.f;

	return 1.f + c3 * Maths::Pow(x - 1.f, 3.f) + c1 * Maths::Pow(x - 1.f, 2.f);
}


float EasingCurveFunctions::ComputeEaseInOutBack(float x)
{
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return x < 0.5f
		? (Maths::Pow(2.f * x, 2.f) * ((c2 + 1.f) * 2.f * x - c2)) * 0.5f
		: (Maths::Pow(2.f * x - 2.f, 2.f) * ((c2 + 1.f) * (x * 2.f - 2.f) + c2) + 2.f) * 0.5f;
}


float EasingCurveFunctions::ComputeEaseInElastic(float x)
{
	const float c4 = Maths::PI2 / 3;

	return x == 0.f
		? 0.f
		: x == 1.f
		? 1.f
		: -Maths::Pow(2.f, 10.f * x - 10.f) * Maths::Sin((x * 10.f - 10.75f) * c4);
}


float EasingCurveFunctions::ComputeEaseOutElastic(float x)
{
	const float c4 = Maths::PI2 / 3.f;

	return x == 0.f
		? 0.f
		: x == 1.f
		? 1.f
		: Maths::Pow(2.f, -10.f * x) * Maths::Sin((x * 10.f - 0.75f) * c4) + 1.f;
}


float EasingCurveFunctions::ComputeEaseInOutElastic(float x)
{
	const float c5 = Maths::PI2 / 4.5f;

	return x == 0.f
		? 0.f
		: x == 1.f
		? 1.f
		: x < 0.5f
		? -(Maths::Pow(2.f, 20.f * x - 10.f) * Maths::Sin((20.f * x - 11.125f) * c5)) * 0.5f
		: (Maths::Pow(2.f, -20.f * x + 10.f) * Maths::Sin((2.f * x - 11.125f) * c5)) * 0.5f + 1.f;
}


float EasingCurveFunctions::ComputeEaseInBounce(float x)
{
	return 1.f - ComputeEaseOutBounce(1.f - x);
}


float EasingCurveFunctions::ComputeEaseOutBounce(float x)
{
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1.f / d1) {
		return n1 * x * x;
	}
	else if (x < 2.f / d1) {
		return n1 * (x -= 1.5f / d1) * x + 0.75f;
	}
	else if (x < 2.5f / d1) {
		return n1 * (x -= 2.25f / d1) * x + 0.9375f;
	}
	else {
		return n1 * (x -= 2.625f / d1) * x + 0.984375f;
	}
}


float EasingCurveFunctions::ComputeEaseInOutBounce(float x)
{
	return x < 0.5f
		? (1.f - ComputeEaseOutBounce(1.f - 2.f * x)) * 0.5f
		: (1.f + ComputeEaseOutBounce(2.f * x - 1.f)) * 0.5f;
}



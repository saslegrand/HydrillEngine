#pragma once

#include "Maths/Maths.hpp"

#include "Generated/Curves.rfkh.h"

enum class HY_ENUM() EasingCurve
{
	EaseInSine,
	EaseOutSine,
	EaseInOutSine,

	EaseInQuad,
	EaseOutQuad,
	EaseInOutQuad,

	EaseInCubic,
	EaseOutCubic,
	EaseInOutCubic,

	EaseInQuart,
	EaseOutQuart,
	EaseInOutQuart,

	EaseInQuint,
	EaseOutQuint,
	EaseInOutQuint,

	EaseInExpo,
	EaseOutExpo,
	EaseInOutExpo,

	EaseInCirc,
	EaseOutCirc,
	EaseInOutCirc,

	EaseInBack,
	EaseOutBack,
	EaseInOutBack,

	EaseInElastic,
	EaseOutElastic,
	EaseInOutElastic,

	EaseInBounce,
	EaseOutBounce,
	EaseInOutBounce,

	None
};


class EasingCurveFunctions
{
public:

	static ENGINE_API std::array<float(*)(float), 30> ComputeCurve;

	template<typename T>
	static inline T Lerp(T start, T end, EasingCurve curve, float param)
	{
		return Maths::Lerp(start, end, ComputeCurve[static_cast<int>(curve)](param));
	}

	static float ComputeEaseInSine(float x);
	static float ComputeEaseOutSine(float x);
	static float ComputeEaseInOutSine(float x);

	static float ComputeEaseInQuad(float x);
	static float ComputeEaseOutQuad(float x);
	static float ComputeEaseInOutQuad(float x);

	static float ComputeEaseInCubic(float x);
	static float ComputeEaseOutCubic(float x);
	static float ComputeEaseInOutCubic(float x);

	static float ComputeEaseInQuart(float x);
	static float ComputeEaseOutQuart(float x);
	static float ComputeEaseInOutQuart(float x);

	static float ComputeEaseInQuint(float x);
	static float ComputeEaseOutQuint(float x);
	static float ComputeEaseInOutQuint(float x);

	static float ComputeEaseInExpo(float x);
	static float ComputeEaseOutExpo(float x);
	static float ComputeEaseInOutExpo(float x);

	static float ComputeEaseInCirc(float x);
	static float ComputeEaseOutCirc(float x);
	static float ComputeEaseInOutCirc(float x);

	static float ComputeEaseInBack(float x);
	static float ComputeEaseOutBack(float x);
	static float ComputeEaseInOutBack(float x);

	static float ComputeEaseInElastic(float x);
	static float ComputeEaseOutElastic(float x);
	static float ComputeEaseInOutElastic(float x);

	static float ComputeEaseInBounce(float x);
	static float ComputeEaseOutBounce(float x);
	static float ComputeEaseInOutBounce(float x);
};

File_Curves_GENERATED
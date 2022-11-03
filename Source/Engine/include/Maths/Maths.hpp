#pragma once
#include <cmath>

#include <Refureku/Refureku.h>

#include "EngineDLL.hpp"
#include "Maths/MathsDef.hpp"

namespace Maths
{
	const float PI			= 3.1415926535897932f;
	const float PI2			= PI * 2.f;
	const float PIHalf		= PI * 0.5f;
	const float RADTODEG	= 180.f / PI;
	const float DEGTORAD	= PI / 180.f;
	const float SmallFloat	= 0.000001f;

	static MATH_INLINE float Sqrt(float val)
	{ 
		return sqrtf(val);
	}

	/**
	* Compute Atan2
	**/
	static MATH_INLINE float Atan2(const float lhs, const float rhs)
	{
		return atan2f(lhs, rhs);
	}

	/**
	* Compute Cosinus
	**/
	static MATH_INLINE float Cos(const float val)
	{
		return cosf(val);
	}


	/**
	* Compute Cosinus
	**/
	static MATH_INLINE float Pow(const float val, const float exp)
	{
		return powf(val, exp);
	}


	/**
	* Compute Sinus
	**/
	static MATH_INLINE float Sin(const float val)
	{
		return sinf(val);
	}

	/**
	* Compute ArcCosinus
	**/
	static MATH_INLINE float Acos(const float val)
	{
		return acosf(val);
	}

	/**
	* Compute ArcSinus
	**/
	static MATH_INLINE float Asin(const float val)
	{
		return asinf(val);
	}

	/**
	* Compute Tan
	**/
	static MATH_INLINE float Tan(const float val)
	{
		return tanf(val);
	}

	/**
	* Check if the value is equal to target with a tolerance
	* @param value : value to check
	* @param target : target to check
	* @param tolerance
	**/
	template <typename T>
	static MATH_INLINE bool Equal(const T& value, const T& target, float tolerance = SmallFloat)
	{
		return fabsf(static_cast<float>(value - target)) <= tolerance;
	}

	/**
	* Give the max value from the type
	**/
	template <typename T>
	static MATH_INLINE  T Max(const T& lhs, const T& rhs)
	{
		return lhs > rhs ? lhs : rhs;
	}

	/**
	* Give the min value from the type
	**/
	template <typename T>
	static MATH_INLINE T Min(const T& lhs, const T& rhs)
	{
		return lhs < rhs ? lhs : rhs;
	}

	/**
	* Clammp the value between min & max
	**/
	template <typename T>
	NO_DISCARD static MATH_INLINE T Clamp(const T val, const T min, const T max)
	{
		return val < min ? min : (val > max ? max : val);
	}

	/**
	* Give the sign of the value
	**/
	template <typename T>
	NO_DISCARD static MATH_INLINE T Sign(const T val)
	{
		return val < (T)0 ? (T)-1 : (val > (T)0 ? (T)1 : (T)0);
	}

	/**
	* Give the absolute value
	**/
	template <typename T>
	static MATH_INLINE T Abs(const T& val)
	{
		return val < (T)0 ? -val : val;
	}

	/**
	* Convert cartesian coordinates to polar
	**/
	static MATH_INLINE void CartesianToPolar(const float x, const float y, float& oRad, float& oAngle)
	{
		oRad = Sqrt(x * x + y * y);
		oAngle = Atan2(y, x);
	}

	/**
	* Convert polar coordinates to polar
	**/
	static MATH_INLINE void PolarToCratesian(const float rad, const float angle, float& oX, float& oY)
	{
		oX = rad * Cos(angle);
		oY = rad * Sin(angle);
	}

	/**
	* Remap value between newMin & newMax
	**/
	template <typename T>
	static MATH_INLINE T LinearConversion(T value, T oldMin, T oldMax, T newMin, T newMax)
	{
		return (value - oldMin) * (newMax - newMin) / (oldMax - oldMin) + newMin;
	}

	/**
	* Lerp value from start to end according to percent
	**/
	template <typename T>
	static MATH_INLINE T Lerp(T start, T end, float percent)
	{
		return start + (end - start) * percent;
	}

	/**
	* Move a value from start to end according to a step
	**/
	template <typename T>
	static MATH_INLINE T MoveToward(T start, T end, float step)
	{
		return Clamp(start + Sign(end - start) * step, Min(start,end), Max(start, end));
	}
}
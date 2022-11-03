#pragma once
#include "EngineDll.hpp"

#include "Generated/Color.rfkh.h"

struct Color4;

struct HY_STRUCT() ENGINE_API Color3
{
	float r, g, b;

	Color3();
	/**
	@brief RGB color constructor (value [0.f,1.f])
	*/
	Color3(const float r, const float g, const float b);

	/**
	@brief RGB color constructor (value [0,255])
	*/
	Color3(const int r, const int g, const int b);

	/**
	@brief RGB color constructor (Color4)
	*/
	Color3(const Color4& c);

	//	Basics

	static const Color3 White;
	static const Color3 LightGray;
	static const Color3 Gray;
	static const Color3 DarkGray;
	static const Color3 Black;

	static const Color3 RawRed;
	static const Color3 RawBlue;
	static const Color3 RawGreen;

	static const Color3 Yellow;
	static const Color3 Orange;
	static const Color3 Brown;
	static const Color3 Red;
	static const Color3 Pink;
	static const Color3 Purple;
	static const Color3 Blue;
	static const Color3 Green;

	static const Color3 LightYellow;
	static const Color3 LightOrange;
	static const Color3 LightBrown;
	static const Color3 LightRed;
	static const Color3 LightPink;
	static const Color3 LightPurple;
	static const Color3 LightBlue;
	static const Color3 LightGreen;

	static const Color3 DarkYellow;
	static const Color3 DarkBrown;
	static const Color3 DarkRed;
	static const Color3 DarkPurple;
	static const Color3 DarkBlue;
	static const Color3 DarkGreen;

	//	Operators

	inline Color3 operator+(const Color3 & a) const;
	inline Color3 operator-(const Color3 & a) const;
	inline Color3 operator*(float a) const;
	inline Color3 operator/(float a) const;

	//	Functions 

	static Color3 Lerp(const Color3 & start, const Color3 & end, float percent);

	Color3_GENERATED
};


struct HY_STRUCT() ENGINE_API Color4
{

	union
	{
		struct { float r, g, b, a; };
		Color3 RGB;
	};

	Color4();

	/**
	@brief RGB color constructor (value [0.f,1.f])
	*/
	Color4(const float r, const float g, const float b);

	/**
	@brief RGBA color constructor (value [0.f,1.f])
	*/
	Color4(const float r, const float g, const float b, const float a);


	/**
	@brief RGBA color constructor (value [0.f,1.f])
	*/
	Color4(Color3 rgb, const float a = 1.0f);

	/**
	@brief RGB color constructor (value [0,255])
	*/
	Color4(const int r, const int g, const int b);

	/**
	@brief RGBA color constructor (value [0,255])
	*/
	Color4(const int r, const int g, const int b, const int a);

	/**
	@brief RGBA color constructor (value [0,255])
	*/
	Color4(Color3 rgb, const int a);

	//	Basics

	static const Color4 White;
	static const Color4 LightGray;
	static const Color4 Gray;
	static const Color4 DarkGray;
	static const Color4 Black;

	static const Color4 RawRed;
	static const Color4 RawBlue;
	static const Color4 RawGreen;

	static const Color4 Yellow;
	static const Color4 Orange;
	static const Color4 Brown;
	static const Color4 Red;
	static const Color4 Pink;
	static const Color4 Purple;
	static const Color4 Blue;
	static const Color4 Green;

	static const Color4 LightYellow;
	static const Color4 LightOrange;
	static const Color4 LightBrown;
	static const Color4 LightRed;
	static const Color4 LightPink;
	static const Color4 LightPurple;
	static const Color4 LightBlue;
	static const Color4 LightGreen;

	static const Color4 DarkYellow;
	static const Color4 DarkBrown;
	static const Color4 DarkRed;
	static const Color4 DarkPurple;
	static const Color4 DarkBlue;
	static const Color4 DarkGreen;

	//	Operators

	inline Color4 operator+(const Color4& a) const;
	inline Color4 operator-(const Color4& a) const;
	inline Color4 operator*(float a) const;
	inline Color4 operator/(float a) const;

	//	Functions 

	static Color4 Lerp(const Color4& start, const Color4& end, float percent);

	Color4_GENERATED
};


#include "Types/Color.inl"

File_Color_GENERATED

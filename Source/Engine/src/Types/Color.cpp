#include "Maths/Maths.hpp"

#include "Generated/Color.rfks.h"

Color3::Color3()
	: r(1.0f), g(1.0f), b(1.0f) {}

Color3::Color3(const float r, const float g, const float b)
	: r(r), g(g), b(b) {}


Color3::Color3(const int r, const int g, const int b)
	: r(r / 255.f), g(g / 255.f), b(b / 255.f) {}

Color3::Color3(const Color4& c)
	: r(c.r), g(c.g), b(c.b) {}

Color3 Color3::Lerp(const Color3& start, const Color3& end, float percent)
{
	Color3 out;
	out.r = Maths::Lerp(start.r, end.r, percent);
	out.g = Maths::Lerp(start.g, end.g, percent);
	out.b = Maths::Lerp(start.b, end.b, percent);

	return out;
}



const Color3  Color3::White			= { 255, 255, 255 };
const Color3  Color3::LightGray		= { 210, 210, 210 };
const Color3  Color3::Gray			= { 128, 128, 128 };
const Color3  Color3::DarkGray		= { 40,  40,  40 };
const Color3  Color3::Black			= { 0,   0,   0 };

const Color3  Color3::RawRed		= { 255,   0,   0 };
const Color3  Color3::RawGreen		= { 0, 255,   0 };
const Color3  Color3::RawBlue		= { 0,   0, 255 };

const Color3  Color3::Yellow		= { 255, 255,   0 };
const Color3  Color3::Orange		= { 255, 155,   0 };
const Color3  Color3::Brown			= { 100,  50,   0 };
const Color3  Color3::Red			= { 255,  50,  50 };
const Color3  Color3::Pink			= { 255, 100, 255 };
const Color3  Color3::Purple		= { 155, 100, 255 };
const Color3  Color3::Blue			= { 0, 100, 255 };
const Color3  Color3::Green			= { 0, 150,  50 };

const Color3  Color3::LightYellow	= { 255, 255, 100 };
const Color3  Color3::LightOrange	= { 255, 150, 100 };
const Color3  Color3::LightBrown	= { 150, 100,  50 };
const Color3  Color3::LightRed		= { 255, 100, 100 };
const Color3  Color3::LightPink		= { 255, 150, 255 };
const Color3  Color3::LightPurple	= { 200, 150, 255 };
const Color3  Color3::LightBlue		= { 150, 200, 255 };
const Color3  Color3::LightGreen	= { 125, 255, 125 };

const Color3  Color3::DarkYellow	= { 180, 120,  20 };
const Color3  Color3::DarkBrown		= { 65,  35,  10 };
const Color3  Color3::DarkRed		= { 150,  40,  40 };
const Color3  Color3::DarkPurple	= { 75,  20, 140 };
const Color3  Color3::DarkBlue		= { 45,  50, 175 };
const Color3  Color3::DarkGreen		= { 25, 100,  25 };


Color4::Color4() 
	: r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}

Color4::Color4(const float r, const float g, const float b)
	: r(r), g(g), b(b), a(1.0f) {}

Color4::Color4(const float r, const float g, const float b, const float a)
	: r(r), g(g), b(b), a(a) {}

Color4::Color4(Color3 rgb, const float a)
	: r(rgb.r), g(rgb.g), b(rgb.b), a(a) {}

Color4::Color4(const int r, const int g, const int b)
	: r(r / 255.f), g(g / 255.f), b(b / 255.f), a(1.0f) {}

Color4::Color4(const int r, const int g, const int b, const int a)
	: r(r / 255.f), g(g / 255.f), b(b / 255.f), a(a / 255.f) {}

Color4::Color4(Color3 rgb, const int a)
	: r(rgb.r / 255.f), g(rgb.g / 255.f), b(rgb.b / 255.f), a(a / 255.f) {}

Color4 Color4::Lerp(const Color4& start, const Color4& end, float percent)
{
	Color4 out;
	out.r = Maths::Lerp(start.r, end.r, percent);
	out.g = Maths::Lerp(start.g, end.g, percent);
	out.b = Maths::Lerp(start.b, end.b, percent);
	out.a = Maths::Lerp(start.a, end.a, percent);

	return out;
}

const Color4  Color4::White			= Color3::White;
const Color4  Color4::LightGray		= Color3::LightGray;
const Color4  Color4::Gray			= Color3::Gray;
const Color4  Color4::DarkGray		= Color3::DarkGray;
const Color4  Color4::Black			= Color3::Black;

const Color4  Color4::RawRed		= Color3::RawRed;
const Color4  Color4::RawGreen		= Color3::RawGreen;
const Color4  Color4::RawBlue		= Color3::RawBlue;

const Color4  Color4::Yellow		= Color3::Yellow;
const Color4  Color4::Orange		= Color3::Orange;
const Color4  Color4::Brown			= Color3::Brown;
const Color4  Color4::Red			= Color3::Red;
const Color4  Color4::Pink			= Color3::Pink;
const Color4  Color4::Purple		= Color3::Purple;
const Color4  Color4::Blue			= Color3::Blue;
const Color4  Color4::Green			= Color3::Green;

const Color4  Color4::LightYellow	= Color3::LightYellow;
const Color4  Color4::LightOrange	= Color3::LightOrange;
const Color4  Color4::LightBrown	= Color3::LightBrown;
const Color4  Color4::LightRed		= Color3::LightRed;
const Color4  Color4::LightPink		= Color3::LightPink;
const Color4  Color4::LightPurple	= Color3::LightPurple;
const Color4  Color4::LightBlue		= Color3::LightBlue;
const Color4  Color4::LightGreen	= Color3::LightGreen;

const Color4  Color4::DarkYellow	= Color3::DarkYellow;
const Color4  Color4::DarkBrown		= Color3::DarkBrown;
const Color4  Color4::DarkRed		= Color3::DarkRed;
const Color4  Color4::DarkPurple	= Color3::DarkPurple;
const Color4  Color4::DarkBlue		= Color3::DarkBlue;
const Color4  Color4::DarkGreen		= Color3::DarkGreen;

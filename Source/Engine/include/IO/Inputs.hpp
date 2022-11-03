#pragma once

#include "Generated/Inputs.rfkh.h"

/**
Keyboard input IDs
*/
enum class HY_ENUM() Key
{
	//	Keyboards
	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,

	Num_0 = 48,
	Num_1 = 49,
	Num_2 = 50,
	Num_3 = 51,
	Num_4 = 52,
	Num_5 = 53,
	Num_6 = 54,
	Num_7 = 55,
	Num_8 = 56,
	Num_9 = 57,

	Space = 32,
	Apostrophe = 39,	/*	'	*/
	Comma = 44,			/*	, 	*/
	Minus = 45,			/*	- 	*/
	Point = 46,			/*	. 	*/
	Slash = 47,			/*	/ 	*/
	BackSlash = 92,		/*  \   */
	SemiColon = 59,		/*	;	*/
	LeftBracket = 91,	/*	[	*/
	RightBracket = 93,	/*	]	*/
	GraveAccent = 96,	/*	`	*/
	Equal = 61,			/*	=	*/

	World1 = 161,	/*	non-US #1	*/
	World2 = 162,	/*	non-US #2	*/

	Escape = 256,
	Enter = 257,
	Tab = 258,
	Backspace = 259,
	Insert = 260,
	Delete = 261,

	ArrowRight = 262,
	ArrowLeft = 263,
	ArrowDown = 264,
	ArrowUp = 265,

	PageUp = 266,
	PageDown = 267,
	Home = 268,
	End = 269,

	CapsLock = 280,
	ScrollLock = 281,
	NumLock = 282,
	PrintScreen = 283,
	Pause = 284,

	F1 = 290,
	F2 = 291,
	F3 = 292,
	F4 = 293,
	F5 = 294,
	F6 = 295,
	F7 = 296,
	F8 = 297,
	F9 = 298,
	F10 = 299,
	F11 = 291,
	F12 = 301,
	F13 = 302,
	F14 = 303,
	F15 = 304,
	F16 = 305,
	F17 = 306,
	F18 = 307,
	F19 = 308,
	F20 = 309,
	F21 = 310,
	F22 = 311,
	F23 = 312,
	F24 = 313,
	F25 = 314,

	//	Num pad

	NumPad_0 = 320,
	NumPad_1 = 321,
	NumPad_2 = 322,
	NumPad_3 = 323,
	NumPad_4 = 324,
	NumPad_5 = 325,
	NumPad_6 = 326,
	NumPad_7 = 327,
	NumPad_8 = 328,
	NumPad_9 = 329,

	NumPad_Decimal = 330,
	NumPad_Divide = 331,
	NumPad_Multiply = 332,
	NumPad_Substract = 333,
	NumPad_Add = 334,
	NumPad_Enter = 335,
	NumPad_Equal = 336,

	LeftShift = 340,
	LeftControl = 341,
	LeftAlt = 342,
	LeftSuper = 343,
	RightShift = 344,
	RightControl = 345,
	RightAlt = 346,
	RightSuper = 347,
	Menu = 348,
};

/**
Mouse button IDs
*/
enum class HY_ENUM() MouseButton
{
	Left    = 0,
	Right   = 1,
	Middle  = 2,
	Side_04 = 3,
	Side_05 = 4,
	Side_06 = 5,
	Side_07 = 6,
	Last    = 7,
};

/**
Mouse cursor mode 
*/
enum class MouseMode
{
	Normal	 = 0x00034001,
	Disabled = 0x00034003,
};

File_Inputs_GENERATED
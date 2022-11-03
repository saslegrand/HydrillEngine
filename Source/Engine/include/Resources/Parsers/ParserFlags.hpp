#pragma once

#include "Generated/ParserFlags.rfkh.h"

enum class HY_ENUM() EImageSTB
{
	IMG_FLIP = 1 << 0,
	IMG_FORCE_GREY = 1 << 1,
	IMG_FORCE_GREY_ALPHA = 1 << 2,
	IMG_FORCE_RGB = 1 << 3,
	IMG_FORCE_RGBA = 1 << 4,
	IMG_GEN_MIPMAPS = 1 << 5,
	IMG_WRAP_REPEAT = 1 << 6,
};

File_ParserFlags_GENERATED
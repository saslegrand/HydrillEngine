#pragma once

#include "EngineDLL.hpp"
#include "Generated/BoolVector3.rfkh.h"

/**
@brief This is a simple container for a vector of 3 booleans. It is mainly used by Rigidbody to define its axis constraints.
*/
struct HY_STRUCT() ENGINE_API BoolVector3
{
	bool X = false, Y = false, Z = false;

	BoolVector3_GENERATED
};

File_BoolVector3_GENERATED

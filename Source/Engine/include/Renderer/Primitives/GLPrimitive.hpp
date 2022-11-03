#pragma once

#include "EngineDLL.hpp"

struct ENGINE_API GLPrimitive
{
protected:
	unsigned int m_ID = 0;

public:
	unsigned int GetID() const;
};
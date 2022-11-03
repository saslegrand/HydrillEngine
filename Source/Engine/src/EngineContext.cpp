#include "EngineContext.hpp"

//	Context definition
EngineContext* EngineContext::m_instance = nullptr;

EngineContext& EngineContext::Instance()
{
	return *m_instance;
}


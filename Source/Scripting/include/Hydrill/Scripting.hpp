#pragma once

// Export symbols for scripting project
#ifdef SCRIPTING_EXPORTS
#define SCRIPTING_API __declspec(dllexport)
#else
#define SCRIPTING_API __declspec(dllimport)
#endif
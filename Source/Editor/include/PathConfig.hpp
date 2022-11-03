#pragma once

#include <Tools/PathConfig.hpp>

#ifdef PUBLISHED
constexpr char INTERNAL_EDITOR_RESOURCES_ROOT[]		= R"(..\Internal\Resources\Editor\)";
constexpr char INTERNAL_EDITOR_RESOURCES_ROOT_RAW[]	= R"(..\Internal\Resources\Editor)";
#else
constexpr char INTERNAL_EDITOR_RESOURCES_ROOT[]		= R"(..\..\..\Internal\Resources\Editor\)";
constexpr char INTERNAL_EDITOR_RESOURCES_ROOT_RAW[]	= R"(..\..\..\Internal\Resources\Editor)";
#endif
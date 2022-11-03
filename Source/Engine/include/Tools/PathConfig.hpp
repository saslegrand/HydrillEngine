#pragma once

#include <string>

#ifdef PUBLISHED
constexpr char ASSETS_ROOT[]						= R"(..\Assets\)";
constexpr char ASSETS_ROOT_RAW[]					= R"(..\Assets)";

constexpr char INTERNAL_ENGINE_RESOURCES_ROOT[]		= R"(..\Internal\Resources\Engine\)";
constexpr char INTERNAL_ENGINE_RESOURCES_ROOT_RAW[] = R"(..\Internal\Resources\Engine)";

constexpr char INTERNAL_ENGINE_SCRIPTING_ROOT[]		= R"(..\Source\Scripting\)";
constexpr char INTERNAL_ENGINE_SCRIPTING_ROOT_RAW[] = R"(..\Source\Scripting)";

constexpr char BINARIES_ROOT[]						= R"(.\)";
constexpr char BINARIES_ROOT_RAW[]					= R"(.\)";

#else
constexpr char ASSETS_ROOT[]						= R"(..\..\..\Assets\)";
constexpr char ASSETS_ROOT_RAW[]					= R"(..\..\..\Assets)";

constexpr char INTERNAL_ENGINE_RESOURCES_ROOT[]		= R"(..\..\..\Internal\Resources\Engine\)";
constexpr char INTERNAL_ENGINE_RESOURCES_ROOT_RAW[] = R"(..\..\..\Internal\Resources\Engine)";

constexpr char INTERNAL_ENGINE_SCRIPTING_ROOT[]		= R"(..\..\..\Source\Scripting\)";
constexpr char INTERNAL_ENGINE_SCRIPTING_ROOT_RAW[] = R"(..\..\..\Source\Scripting)";

#ifdef _DEBUG
constexpr char BINARIES_ROOT[] = R"(..\..\..\Binaries\Debug\)";
constexpr char BINARIES_ROOT_RAW[] = R"(..\..\..\Binaries\Debug)";
#else
constexpr char BINARIES_ROOT[] = R"(..\..\..\Binaries\Release\)";
constexpr char BINARIES_ROOT_RAW[] = R"(..\..\..\Binaries\Release)";
#endif

#endif
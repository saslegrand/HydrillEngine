#pragma once

#include "Maths/Vector3.hpp"
#include "Types/Color.hpp"

#include "Generated/Light.rfkh.h"

enum class HY_ENUM() LightType : std::uint32_t
{
	Directional = 0,
	Point		 = 1,
	Spot		 = 2
};

struct Light
{
	alignas(16) Vector3 position = Vector3::Zero;
	alignas(16) Vector3 direction = Vector3::Down;

	alignas(16) Color3  diffuse = Color3::White;
	alignas(16) Color3  ambient = Color3(0.1f, 0.1f, 0.1f);
	alignas(16) Color3  specular = Color3::White;

	alignas(8)  Vector2 cutOff = Vector2(0.95f, 0.90f);

	alignas(4)  float radius = 5.f;
	alignas(4)  float intensity = 1.0f;

	alignas(4)  LightType type = LightType::Point;

	alignas(4)  std::uint32_t castShadows = true;
	alignas(4)  std::uint32_t enabled     = true;
};


File_Light_GENERATED
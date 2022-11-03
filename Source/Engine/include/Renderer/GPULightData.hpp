#pragma once
#include "Maths/Vector3.hpp"
#include "Types/Color.hpp"

struct GPULightData
{
    Vector3 position  = Vector3::Zero;
    Vector3 direction = Vector3::Down;

    Color4   diffuse;
    Color4   ambient;
    Color4   specular;

    Vector3 attenuation;

    int  type;
};
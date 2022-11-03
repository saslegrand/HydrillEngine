#pragma once

#include "Generated/SoundEnums.rfkh.h"

enum class HY_ENUM() ESoundAttenuationModel
{
    NONE = 0,        /* No distance attenuation and no spatialization. */
    INVERSE = 1,        /* Equivalent to OpenAL's AL_INVERSE_DISTANCE_CLAMPED. */
    LINEAR = 2,        /* Linear attenuation. Equivalent to OpenAL's AL_LINEAR_DISTANCE_CLAMPED. */
    EXPONENTIAL = 3         /* Exponential attenuation. Equivalent to OpenAL's AL_EXPONENTIAL_DISTANCE_CLAMPED. */
};

File_SoundEnums_GENERATED
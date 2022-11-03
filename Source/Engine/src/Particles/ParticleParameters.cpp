#include "Particles/ParticleParameters.hpp"

#include "Generated/ParticleParameters.rfks.h"

ParticleConstantParam1f::ParticleConstantParam1f()
{
	value = 1.f;
}

float ParticleConstantParam1f::GetValue()
{
	return value;
}

void ParticleConstantParam1f::Initialize()
{
	if (valueMin < valueMax)
	{
		value = valueMin + static_cast<float>(rand() / (static_cast<float>(RAND_MAX / (valueMax - valueMin))));
	}
	else if (valueMin == valueMax)
	{
		value = valueMax;
	}
	else
	{
		value = 1.f;
	}
}


ParticleParam1f::ParticleParam1f()
{
	value     = 1.f;
	curveType = EasingCurve::None;
}

float ParticleParam1f::GetValueFromCurve(float x)
{
	if (curveType == EasingCurve::None) return ParticleConstantParam1f::GetValue();
	
	float val = x;

	if (format.TestBit(ECurveFormat::Symmetrical)) val = Maths::Sin(val * Maths::PI);
	if (format.TestBit(ECurveFormat::Inverted)) val = 1.0f - val;

	return EasingCurveFunctions::ComputeCurve[static_cast<int>(curveType)](val) * value;
}

void ParticleParam1f::Initialize()
{
	ParticleConstantParam1f::Initialize();
}


ParticleParameters::ParticleParameters()
{

}

void ParticleParameters::Initalize()
{
	lifeStart.Initialize();
	burstForce.Initialize();
	airDragging.Initialize();
	mass.Initialize();
	sizeScale.Initialize();
	billboardRotationSpeed.Initialize();
	emissiveStrength.Initialize();
}

void ParticleParameters::Update(float ageRatio)
{

}
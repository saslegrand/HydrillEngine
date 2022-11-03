#pragma once

#include "Maths/Vector3.hpp"
#include "Tools/Curves.hpp"
#include "Tools/Flags.hpp"
#include "Types/Color.hpp"
#include "Core/baseObject.hpp"

#include "Generated/ParticleParameters.rfkh.h"

//	Parameters structures

#pragma region ___FloatParam___

enum class HY_ENUM() ECurveFormat
{
	Inverted = 1 << 0,
	Symmetrical = 1 << 1,
};


struct HY_STRUCT() ENGINE_API ParticleConstantParam1f : public BaseObject
{
private:

public:
	HY_FIELD() float valueMin = 1.f;
	HY_FIELD() float valueMax = 1.f;
	float value = 1.f;

	ParticleConstantParam1f();
	float GetValue();

	virtual void Initialize();

	ParticleConstantParam1f_GENERATED
};

struct HY_STRUCT() ENGINE_API ParticleParam1f : public ParticleConstantParam1f
{
public:

	HY_FIELD() EasingCurve curveType = EasingCurve::EaseOutCirc;

#pragma warning(disable:4251)
	HY_FIELD() Flags<ECurveFormat> format;
#pragma warning(default:4251)
	
	ParticleParam1f();

	float GetValueFromCurve(float x);
	void Initialize() override;

	ParticleParam1f_GENERATED
};

#pragma endregion


struct HY_STRUCT() ENGINE_API ParticleParameters : public BaseObject
{

//	Variables

	HY_FIELD() ParticleConstantParam1f lifeStart;

	HY_FIELD() ParticleConstantParam1f burstForce;

	HY_FIELD() ParticleParam1f sizeScale;
	HY_FIELD() ParticleParam1f airDragging;
	HY_FIELD() ParticleParam1f mass;
	HY_FIELD() ParticleParam1f emissiveStrength;

	HY_FIELD() ParticleParam1f billboardRotationSpeed;

	HY_FIELD() Color4 startColor = Color4::White;
	HY_FIELD() Color4 endColor = Color4::White;

	HY_FIELD() Color3 startEmissiveColor = Color3::White;
	HY_FIELD() Color3 endEmissiveColor = Color3::White;

//	Shader variables


//	Constructors

	ParticleParameters();

//	Functions

	void Initalize();
	void Update(float ageRatio);

	ParticleParameters_GENERATED
};

File_ParticleParameters_GENERATED
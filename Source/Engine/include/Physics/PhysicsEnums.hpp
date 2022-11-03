#pragma once

#include <physx/geometry/PxGeometry.h>
#include <physx/PxShape.h>
#include <physx/PxForceMode.h>
using namespace physx;

#include "EngineDLL.hpp"
#include "Generated/PhysicsEnums.rfkh.h"

/**
	@brief Flag for collider's shape, if it's trigger or not, and if we want to visual it in PhysxVisualDebuger. Wrapper of PxFlags<PxShapeFlag::Enum>
*/
typedef PxFlags<PxShapeFlag::Enum, PxU8> ColliderFlag;


/**
	@brief Enum of all colliders shapes possible. Wrapper of PxGeometryType::Enum
*/
typedef PxGeometryType::Enum ColliderShapeType;


/**
	@brief Enum of all force modes. Wrapper of PxForceMode::Enum
*/
typedef PxForceMode::Enum ForceMode;


/**
@brief Enum InterpolationMode. None is the default method, interpolate give better results but at a performance cost, extrapolate adjust position based on velocity
*/
enum class HY_ENUM() InterpolationMode
{
	None,
	Interpolate,
	Extrapolate
};

/**
@brief Enum InterpolationMode. Discrete is the default method, others activate Complex Collisions Detection at high velocity
*/
enum class HY_ENUM() CollisionDetectionMode
{
	Discrete,
	Continuous,
	ContinuousSpeculative
};

enum PhysicsLayer
{
	Layer1 = (1 << 0),
	Layer2 = (1 << 1),
	Layer3 = (1 << 2),
	Layer4 = (1 << 3),
	Layer5 = (1 << 4),
	Layer6 = (1 << 5),
	Layer7 = (1 << 6),
	Layer8 = (1 << 7),
};

File_PhysicsEnums_GENERATED
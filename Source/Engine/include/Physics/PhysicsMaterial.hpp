#pragma once

#include <physx/PxMaterial.h>
using namespace physx;

#include "Core/BaseObject.hpp"

#include "Generated/PhysicsMaterial.rfkh.h"

/**
@brief Material used for the physics simulation. Used by a Collider or a Rigidbody
*/
struct HY_STRUCT() ENGINE_API PhysicsMaterial : public BaseObject
{
	/**
	* @brief staticFriction is the amount of friction when the object is not moving and is starting to move
	* */
	HY_FIELD() float staticFriction = 0.02f;

	/**
	* @brief dynamicFriction is the amout of friction when the object is moving
	* */
	HY_FIELD() float dynamicFriction = 0.02f;

	/**
	* @brief restitution is the amount of bounciness on a collision
	* */
	HY_FIELD() float restitution = 0.02f;

	/**
	* @brief combineMode is the operation done between two physics materials to get the friction of this object
	* */
	PxCombineMode::Enum combineMode = PxCombineMode::Enum::eAVERAGE;

	PhysicsMaterial_GENERATED
};

File_PhysicsMaterial_GENERATED
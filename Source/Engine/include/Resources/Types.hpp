#pragma once

#include "Maths/Vector3.hpp"
#include "Maths/Vector2.hpp"

struct Vertex
{
	Vector3 position = { 0, 0, 0 };
	Vector3 normal = { 0, 0, 0 };

	Vector2 uvs = { 0, 0 };

	Vector3 tangent = { 0, 0, 0 };
	Vector3 bitangent = { 0, 0, 0 };
};


/**
@brief Defines how many bones can influence a vertex position (and they should be weighted).
1 will make the vertex influenced only by the nearest bone, 4 will potentially take account of more bones and 4 is a good max for performances.
In therory should be settable in SkeletalMeshComponent.
*/
#define MAX_BONE_INFLUENCE 4

struct SkeletalVertex
{
	Vector3 position = { 0, 0, 0 };
	Vector3 normal = { 0, 0, 0 };

	Vector2 uvs = { 0, 0 };

	Vector3 tangent = { 0, 0, 0 };
	Vector3 bitangent = { 0, 0, 0 };

	int boneIDs[MAX_BONE_INFLUENCE] = { 0 };
	float weights[MAX_BONE_INFLUENCE] = { 0.0f };
};
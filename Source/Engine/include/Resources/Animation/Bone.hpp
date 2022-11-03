#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Maths/Matrix4.hpp"
#include "ECS/Transform.hpp"

class Bone
{
private:
	Bone* m_parent = nullptr;
	//std::vector<std::unique_ptr<Bone>> m_children;

public:
	std::string m_name = "";
	unsigned int m_index = 0;


	// Shouldn't we have a Transform class separated from a TransformComponent class so we can manipulate Transforms easily ?
	TransformData m_transformData;
	Matrix4 m_localTransform;
	Matrix4 m_worldTransform;



	Bone() = default;

	bool TryAddBoneRecursively(const Bone& newBone, unsigned int parentBoneIndex);

	void ComputeWorldTransform();

};
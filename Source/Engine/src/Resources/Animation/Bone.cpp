#include "Resources/Animation/Bone.hpp"

bool Bone::TryAddBoneRecursively(const Bone& newBone, unsigned int parentBoneIndex)
{
	//if (parentBoneIndex == m_index)
	//{
	//	m_children.push_back(std::make_unique<Bone>(newBone));
	//	Bone* bone = m_children.back().get();
	//	bone->m_parent = this;
	//	bone->ComputeWorldTransform();
	//}
	//
	//for (size_t i = 0; i < m_children.size(); i++)
	//{
	//	if (m_children[i].get()->TryAddBoneRecursively(newBone, parentBoneIndex))
	//	{
	//
	//	}
	//}

	return true;
}

void Bone::ComputeWorldTransform()
{

}
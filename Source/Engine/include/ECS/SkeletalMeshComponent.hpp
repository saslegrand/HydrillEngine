#pragma once

#include "ECS/Gameobject.hpp"

#include "Resources/Resource/SkeletalMesh.hpp"
#include "Resources/Resource/Material.hpp"
//#include "Renderer/ShaderType.hpp"
//#include "Renderer/MeshSurface.hpp"

#include "Generated/SkeletalMeshComponent.rfkh.h"

/**
@brief SkeletalMesh component, holds a SkeletalMesh with a material to be drawn
*/
class HY_CLASS() ENGINE_API SkeletalMeshComponent : public Component
{
private:

	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<SkeletalMeshComponent> defaultInstantiator(GameObject& GO, const HYGUID& uid);

//	Variables

private:

	HY_FIELD() SkeletalMesh* m_mesh = nullptr;

	/*HY_FIELD() ShaderType m_shaderType = ShaderType::PBR;
	HY_FIELD() MeshSurface m_meshSurface = MeshSurface::OPAQUE;*/

public:

	HY_FIELD() HyVector<Material*> materials;
	HY_FIELD() GameObject* skinRoot;
	Matrix4 globalInverseTransform;

//	Constructors & Destructors

public:

	SkeletalMeshComponent(GameObject& gameObject, const HYGUID& uid);
	~SkeletalMeshComponent();


//	Functions

private:
	void AddLinkedBoneMatrix(GameObject* currentNode, SkeletalData& skeletalData, int number);

	/**
	@brief First called function after the component was loaded
	*/
	void Initialize();

public:

	/**
	@brief
	*/
	void ComputeBoneMatrices();

	/**
	@brief Set a new skeletal mesh to draw

	@param newMesh : New skeletal mesh to draw
	*/
	void SetMesh(SkeletalMesh* newMesh);

	/**
	@brief Activate/Deactivate the component

	@param value : Activate if true, deactivate otherwise
	*/
	void SetActive(bool value) override;

	/**
	@brief Get the mesh attached

	@return SkeletalMesh* : SkeletalMesh attached
	*/
	SkeletalMesh* GetMesh() const;

	void OnModify() override;

	SkeletalMeshComponent_GENERATED
};

File_SkeletalMeshComponent_GENERATED
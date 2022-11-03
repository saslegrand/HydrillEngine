#pragma once

#include "ECS/Gameobject.hpp"

#include "Resources/Resource/Mesh.hpp"
#include "Resources/Resource/Material.hpp"
//#include "Renderer/ShaderType.hpp"
//#include "Renderer/MeshSurface.hpp"

#include "Generated/MeshComponent.rfkh.h"


/**
@brief Mesh component, holds a mesh with a material to be drawn
*/
class HY_CLASS() ENGINE_API MeshComponent : public Component
{
private:

	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<MeshComponent> defaultInstantiator(GameObject& GO, const HYGUID& uid);

//	Variables

private:

	HY_FIELD() Mesh* m_mesh = nullptr;

public:

	HY_FIELD() HyVector<Material*> materials;

//	Constructors & Destructors

public:

	MeshComponent(GameObject& gameObject, const HYGUID& uid);
	MeshComponent(MeshComponent const& cam) = delete;
	MeshComponent(MeshComponent&& cam) = default;

	MeshComponent& operator=(MeshComponent const& cam) = delete;
	MeshComponent& operator=(MeshComponent && cam) = default;

	~MeshComponent();

//	Functions

private : 

	/**
	@brief First called function after the component was loaded
	*/
	void Initialize();

public:
	/**
	Activate/Deactivate the component

	@param value : activate if true, deactivate otherwise
	*/
	void SetActive(bool value) override;

	/**
	@brief Set a new mesh to draw

	@param newMesh : New mesh to draw
	*/
	void SetMesh(Mesh* newMesh);

	/**
	@brief Get the mesh attached

	@return Mesh* : Mesh attached
	*/
	Mesh* GetMesh() const;

	/**
	@brief Called when modified on inspector
	*/
	void OnModify() override;

	MeshComponent_GENERATED
};

File_MeshComponent_GENERATED
#pragma once

#include <vector>

#include "Resources/Resource/Prefab.hpp"
#include "Resources/Resource/Material.hpp"
#include "Resources/Resource/Mesh.hpp"
#include "ECS/TransformData.hpp"

#include "Generated/Model.rfkh.h"

/**
@brief Model is a prefab of meshes ready to be instantiated
*/
class HY_CLASS() Model : public Prefab
{
private:
	HY_METHOD(rfk::Instantiator)
		static rfk::UniquePtr<Model> defaultInstantiator(const HYGUID & uid);

private:
	
	BoundingBox m_boundingBox;

protected:
	std::vector<Mesh*> m_meshes = {};
	std::vector<TransformData> m_meshesOffsets = {};
	std::vector<Material*> m_materials = {};

public:
	std::string m_originalPath = "";


	ENGINE_API Model(const HYGUID& uid);

	/*
	@brief Get the list of meshes of the model
	@return std::vector<Mesh*>& : meshes
	*/
	ENGINE_API const std::vector<Mesh*>& GetMeshes() const;

	/*
	@brief Get the list of transform meshes of the model
	@return std::vector<TransformData&>& : meshes transform
	*/
	ENGINE_API const std::vector<TransformData>& GetMeshesOffsets() const;

	/*
	@brief Get the list of materials of the model
	@return std::vector<Material*>& : materials
	*/
	ENGINE_API const std::vector<Material*>& GetMaterials() const;


	/*
	@brief Add a mesh to the model (should only be used by loader)
	*/
	void AddMesh(Mesh* mesh, const TransformData& offset = TransformData());

	virtual bool Import(const std::string& path) override;
	virtual void LoadInGPUMemory() override {}
	virtual void UnloadFromGPUMemory() override {}

	virtual void Serialize() override;
	virtual void Deserialize() override;

	void AddMaterial(Material& material);

	ENGINE_API const BoundingBox& GetBoudingBox() const;
	void SetBoudingBox(const Vector3& min, const Vector3& max);
	void ComputeBoudingBox();

	Model_GENERATED
};

File_Model_GENERATED
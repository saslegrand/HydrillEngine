#pragma once

#include <Refureku/TypeInfo/Archetypes/Struct.h>
#include <unordered_map>
#include <memory>
#include <deque>

#include "EngineDll.hpp"
#include "Renderer/MaterialSurface.hpp"
#include "Renderer/ShaderType.hpp"
#include "Renderer/MeshInstance.hpp"

class SkeletalMeshComponent;
class MeshComponent;
class SkeletalMesh;
class GameObject;
class Material;
class HYGUID;
class Mesh;

/**
@brief 
*/
class MeshSystem
{
private:

	struct MaterialPipeline
	{
		ShaderType shader;
		MaterialSurface surface;
	};

	struct Instances
	{
		std::vector<MaterialPipeline> materialsPipelines;

		bool registered = false;
	};


	struct SubmeshesInstances : public Instances
	{
		std::deque<MeshInstance> instances;

		Mesh* currentMesh = nullptr;
	};

	struct SubSkeletalMeshesInstances : public Instances
	{
		std::deque<SkeletalMeshInstance> instances;

		SkeletalMesh* currentMesh = nullptr;
	};


//	Variables

private:

	std::vector<std::unique_ptr<MeshComponent>> m_meshComponents;
	std::vector<std::unique_ptr<SkeletalMeshComponent>> m_skeletakMeshComponents;

	std::unordered_map<MeshComponent*, SubmeshesInstances> m_meshCompInstances;
	std::unordered_map<Material*, std::vector<MeshInstance*>> m_meshInstancesFromMat;

	std::unordered_map<SkeletalMeshComponent*, SubSkeletalMeshesInstances> m_skMeshCompInstances;
	std::unordered_map<Material*, std::vector<SkeletalMeshInstance*>> m_skeletalMeshInstancesFromMat;

//	Functions

private:

	/**
	@brief Send (skeletal)mesh instances to the render system

	@param pair of (skeletal)mesh Component and a sub-(skeletal)mesh instances
	*/
	template<typename TPairComponentInstances>
	void SendInstancesToRenderSystem(TPairComponentInstances& componentInstances);

	/**
	@brief Generate sub meshes instances from a mesh component

	@param meshComp : mesh component to generate the sub meshes instances from

	@return true if the generation is a success
	*/
	bool GenerateSubMeshesInstances(MeshComponent& meshComp);

	/**
	@brief Degenerate sub meshes instances from a mesh component

	@param skMeshComp : mesh component to degenerate the sub meshes instances from
	*/
	void DegenerateSubMeshesInstances(MeshComponent& meshComp);

	/**
	@brief Generate sub skeletal meshes instances from a skeletal mesh component

	@param skMeshComp : skeletal mesh component to generate the sub skeletal meshes instances from

	@return true if the generation is a success
	*/
	bool GenerateSubMeshesInstances(SkeletalMeshComponent& skMeshComp);

	/**
	@brief Degenerate sub skeletal meshes instances from a skeletal mesh component

	@param skMeshComp : skeletal mesh component to degenerate the sub skeletal meshes instances from
	*/
	void DegenerateSubMeshesInstances(SkeletalMeshComponent& skMeshComp);

public:

	/**
	@brief Initialize the mesh system
	*/
	void Initialize() {};

	/**
	@brief Update mesh system and send unregistered meshes to the render system if possible.
	This is a dirty functions, if a better method is ossible try it !!
	*/
	void Update();

	/**
	@brief Update sub mesh instances given by the given mesh component

	@param meshComp : mesh component to update in the renderer
	*/
	void UpdateMeshComponent(MeshComponent& meshComp);

	/**
	@brief Update sub skeletal mesh instances given by the given skeletal mesh component

	@param skMeshComp : skeletal mesh component to update in the renderer
	*/
	void UpdateSkeletalMeshComponent(SkeletalMeshComponent& skMeshComp);

	/**
	@brief Update meshes instances (skeletal or not) binded to the given material 

	@param material : updated material
	*/
	ENGINE_API void UpdateMeshesWithMaterial(Material* material);


	/**
	@brief Send all sub meshes of a mesh to the render system

	@param subMeshinstances : packed instances to send
	*/
	void SendToRenderSystem(SubmeshesInstances& subMeshinstances);

	/**
	@brief Remove all sub meshes of a mesh from the render system

	@param subMeshinstances : packed instances to remove
	*/
	void RemoveFromRenderSystem(SubmeshesInstances& subMeshinstances);


	/**
	@brief Send all sub skeletal meshes of a skeletal mesh to the render system

	@param subSkeletalMeshinstances : packed instances to send
	*/
	void SendToRenderSystem(SubSkeletalMeshesInstances& subSkeletalMeshinstances);

	/**
	@brief Remove all sub skeletal meshes of a skeletal mesh from the render system

	@param subSkeletalMeshinstances : packed instances to remove
	*/
	void RemoveFromRenderSystem(SubSkeletalMeshesInstances& subSkeletalMeshinstances);

	/**
	@brief Add a new mesh component to the system

	@param owner : Owner of the mesh component
	@param id : Unique identifier of the mesh component

	@return Behavior* : Created mesh component, nullptr if not valid
	*/
	MeshComponent* AddMeshInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id);

	/**
	@brief Remove an existing mesh component in the system

	@param mesh component : mesh component to remove
	*/
	void RemoveMeshInstance(MeshComponent& meshComp);

	/**
	@brief Add a new skmesh component to the system

	@param owner : Owner of the skmesh component
	@param id : Unique identifier of the skmesh component

	@return Behavior* : Created skmesh component, nullptr if not valid
	*/
	SkeletalMeshComponent* AddSkMeshInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id);

	/**
	@brief Remove an existing skmesh component in the system

	@param skmesh component : skmesh component to remove
	*/
	void RemoveSkMeshInstance(SkeletalMeshComponent& skMeshComp);
};
#include <Renderer/MeshInstance.hpp>

#include "ECS/Transform.hpp"
#include "Resources/Resource/Mesh.hpp"
#include "Resources/Resource/SkeletalMesh.hpp"


MeshInstance::MeshInstance(MeshData* mesh, Material* material, Transform* transform)
	: mesh(mesh), material(material), transform(transform) {}

SkeletalMeshInstance::SkeletalMeshInstance(MeshData* mesh, Material* material, Transform* transform, SkeletalData* skeleton)
	: MeshInstance(mesh, material,transform), skeleton(skeleton) {}
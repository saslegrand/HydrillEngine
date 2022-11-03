#pragma once

#include <vector>
#include <unordered_map>

#include <Resources/Resource/Resource.hpp>
#include <Renderer/RenderObjects/Camera.hpp>
#include <Renderer/Primitives/FrameBuffer.hpp>

class Resource;
class Material;
class Mesh;
class SkeletalMesh;
class Model;

struct MeshData;
struct BoundingBox;

class RenderPreview
{

//	Variables

private:

	unsigned int m_dimensions = 1024;
	std::vector<const Material*>	 m_materialsToRenderPreview;
	std::vector<const Mesh*>		 m_meshesToRenderPreview;
	std::vector<const SkeletalMesh*> m_skeletalMeshesToRenderPreview;
	std::vector<const Model*>		 m_modelsToRenderPreview;
	Camera m_camera;

	MeshData* m_materialSphereMesh;

	std::unordered_map<const Resource*, unsigned int> m_textures;
	FrameBuffer m_framebuffer;

public:

	~RenderPreview();

//	Functions

private:

	void RenderMaterials();
	void RenderMeshes();
	void RenderSkeletalMeshes();
	void RenderModels();

	Matrix4 GetCenteredModelMatrix(const BoundingBox& aabb);

	unsigned int AddTexture(const Resource& resource);

	template <DerivedResource TResource>
	unsigned int AddResourceInQueue(const TResource& resource, std::vector<const TResource*>& queue);

public :

	void Initialize();
	void Render();

	unsigned int AddResourceInQueue(const Resource& resource);
};
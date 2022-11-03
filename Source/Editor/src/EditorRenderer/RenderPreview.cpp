#include "EditorRenderer/RenderPreview.hpp"

#include <glad/gl.h>

#include <Renderer/RenderSystem.hpp>
#include <Resources/Resource/Material.hpp>
#include <Resources/Resource/Mesh.hpp>
#include <Resources/Resource/SkeletalMesh.hpp>
#include <Resources/Resource/Model.hpp>
#include <Resources/ResourcesManager.hpp>
#include <Tools/PathConfig.hpp>
#include <EngineContext.hpp>

#include "EditorResourcePreview.hpp"
#include "EditorContext.hpp"

constexpr float cameraPosZ = 1.25f;

std::vector<Light> ligths =
{
	{
		Vector3::Zero,
		Vector3(-1.0f,-5.0f,-1.0f).SafeNormalized(),
		Color3::White,
		Color3(0.1f,0.1f,0.1f),
		Color3::White,
		Vector2(0.95f,0.90f),
		5.f,
		2.5f,
		LightType::Directional,
		false,
		true
	}
};

RenderPreview::~RenderPreview()
{
	// Delete generated textures
	for (auto [key, texture] : m_textures)
		glDeleteTextures(1, &texture);
}

void RenderPreview::Initialize()
{
	Mesh* sphere = static_cast<Mesh*>(EngineContext::Instance().resourcesManager->GetResourceByPath(std::string(INTERNAL_ENGINE_RESOURCES_ROOT) + std::string(R"(Objects\SphereInternal.mesh)")));

	m_materialSphereMesh = sphere ? &sphere->subMeshes[0] : nullptr;

	m_camera.SetAspect(1.0f);
	m_camera.SetTransform({ 0.0f,0.0f,cameraPosZ }, Quaternion::Identity);
	m_camera.Update();

	// Create the framebuffer and set default values
	m_framebuffer = FrameBuffer(FrameBuffer::InternalFormat::RGBA, FrameBuffer::Format::RGBA);
	m_framebuffer.SetDimensions(static_cast<float>(m_dimensions), static_cast<float>(m_dimensions));
	m_framebuffer.Create();
}

unsigned int RenderPreview::AddTexture(const Resource& resource)
{
	// Create the new texture
	unsigned int textureID;
	glGenTextures(1, &textureID);

	// Register the texture
	m_textures.emplace(
		std::piecewise_construct,
		std::make_tuple(&resource),
		std::make_tuple(textureID)
	);

	return textureID;
}

template <DerivedResource TResource>
unsigned int RenderPreview::AddResourceInQueue(const TResource& resource, std::vector<const TResource*>& queue)
{
	auto it = std::find(queue.begin(), queue.end(), &resource);

	if (it != queue.end()) return 0;

	// Add waiting texture generation
	queue.emplace_back(&resource);

	// Create framebuffer texture
	return AddTexture(resource);
}

unsigned int RenderPreview::AddResourceInQueue(const Resource& resource)
{
	if (resource.getArchetype() == Material::staticGetArchetype())		return AddResourceInQueue(static_cast<const Material&>(resource), m_materialsToRenderPreview);
	if (resource.getArchetype() == SkeletalMesh::staticGetArchetype())	return AddResourceInQueue(static_cast<const SkeletalMesh&>(resource), m_skeletalMeshesToRenderPreview);
	if (resource.getArchetype() == Mesh::staticGetArchetype())			return AddResourceInQueue(static_cast<const Mesh&>(resource), m_meshesToRenderPreview);
	if (resource.getArchetype() == Model::staticGetArchetype())		    return AddResourceInQueue(static_cast<const Model&>(resource), m_modelsToRenderPreview);

	return 0;
}

const MeshShader& GetMeshShader(ShaderType type)
{
	RenderSystem& renderSystem = SystemManager::GetRenderSystem();

	switch (type)
	{
	case ShaderType::PBR: return renderSystem.pbrShader;

	case ShaderType::UNLIT: return renderSystem.unlitShader;

		//	Return lit shader by default
	default: return renderSystem.litShader;
	}
}

void RenderPreview::RenderMaterials()
{
	if (!m_materialSphereMesh) return;

	glBindVertexArray(SystemManager::GetRenderSystem().GPUWrapper.meshVAO.GetID());

	std::sort(m_materialsToRenderPreview.begin(), m_materialsToRenderPreview.end(), [](Material const* mat0, Material const* mat1) {
		return mat0->surface < mat1->surface;
		});

	bool isntBlend = true;
	for (const Material* mat : m_materialsToRenderPreview)
	{
		if (!mat) continue;

		if (isntBlend)
		{
			if (mat->surface == MaterialSurface::TRANSLUCENT)
			{
				isntBlend = false;

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
		}

		m_framebuffer.AttachNewTexture(m_textures[mat]);
		m_framebuffer.Bind();

		glViewport(0, 0, m_dimensions, m_dimensions);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const MeshShader& shader = GetMeshShader(mat->shader);

		shader.BindMeshShader();
		shader.DrawMesh(Matrix4::Identity, *m_materialSphereMesh, &mat->data);
		shader.UnbindShader();

		m_framebuffer.EndDraw();
		m_framebuffer.Unbind();
	}

	if (!isntBlend)
		glDisable(GL_BLEND);

	glBindVertexArray(0);

	m_materialsToRenderPreview.clear();
}


Matrix4 RenderPreview::GetCenteredModelMatrix(const BoundingBox& aabb)
{
	//	Define object scale depending on its AABB dimensions
	Vector3 dim = aabb.max - aabb.min;
	float meshMaxDimension = Maths::Max(Maths::Max(dim.x, dim.y), dim.z);
	float scale = 1.f / meshMaxDimension;

	//	Recenter the object depending on its scale
	Vector3 centerPosition = (aabb.min + dim * 0.5f) * scale;
	Vector3 position = Vector3::Zero - centerPosition;

	float maxZPos = centerPosition.z + scale * 0.5f;
	position.z -= 1.25f - (cameraPosZ - maxZPos);

	return Matrix4::Translate(position) * Matrix4::Scale(Vector3::One * scale);
}

void RenderPreview::RenderMeshes()
{
	RenderSystem& renderSystem = SystemManager::GetRenderSystem();

	glBindVertexArray(renderSystem.GPUWrapper.meshVAO.GetID());
	renderSystem.litShader.BindMeshShader();

	for (const Mesh* mesh : m_meshesToRenderPreview)
	{
		const BoundingBox& aabb = mesh->GetBoudingBox();

		//	Set model matrix
		Matrix4 modelMatrix = GetCenteredModelMatrix(aabb);

		m_framebuffer.AttachNewTexture(m_textures[mesh]);
		m_framebuffer.Bind();

		glViewport(0, 0, m_dimensions, m_dimensions);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (const MeshData& subMesh : mesh->subMeshes)
		{
			MaterialData const* data = subMesh.material ? &subMesh.material->data : nullptr;
			SystemManager::GetRenderSystem().litShader.DrawMesh(modelMatrix, subMesh, data);
		}

		m_framebuffer.EndDraw();
		m_framebuffer.Unbind();
	}

	renderSystem.litShader.UnbindShader();
	glBindVertexArray(0);

	m_meshesToRenderPreview.clear();
}


void RenderPreview::RenderSkeletalMeshes()
{
	RenderSystem& renderSystem = SystemManager::GetRenderSystem();

	glBindVertexArray(renderSystem.GPUWrapper.skeletalMeshVAO.GetID());

	renderSystem.litShader.BindSkeletalMeshShader();

	for (const SkeletalMesh* skMesh : m_skeletalMeshesToRenderPreview)
	{
		const BoundingBox& aabb = skMesh->GetBoudingBox();

		//	Set model matrix
		Matrix4 modelMatrix = GetCenteredModelMatrix(aabb);

		m_framebuffer.AttachNewTexture(m_textures[skMesh]);
		m_framebuffer.Bind();

		glViewport(0, 0, m_dimensions, m_dimensions);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int i = 0;
		for (const MeshData& subMesh : skMesh->subMeshes)
		{
			const SkeletalData* skeleton = i < skMesh->skeletonDatas.size() ? &skMesh->skeletonDatas[i] : nullptr;

			if (skeleton) renderSystem.litShader.DrawSkeletalMesh(modelMatrix, subMesh, *skeleton, nullptr);

			i++;
		}

		m_framebuffer.EndDraw();
		m_framebuffer.Unbind();
	}

	renderSystem.litShader.UnbindShader();
	glBindVertexArray(0);

	m_skeletalMeshesToRenderPreview.clear();
}


void RenderPreview::RenderModels()
{
	RenderSystem& renderSystem = SystemManager::GetRenderSystem();


	for (const Model* model : m_modelsToRenderPreview)
	{
		const BoundingBox& aabb = model->GetBoudingBox();

		//	Set model matrix
		Matrix4 modelMatrix = GetCenteredModelMatrix(aabb);

		m_framebuffer.AttachNewTexture(m_textures[model]);
		m_framebuffer.Bind();

		glViewport(0, 0, m_dimensions, m_dimensions);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int meshIndex = 0;
		auto meshOffsets = model->GetMeshesOffsets();

		for (const Mesh* mesh : model->GetMeshes())
		{
			//	Get model material once here
			const auto& materials = model->GetMaterials();
			size_t matCount = materials.size();

			Matrix4 worldModelMatrix = modelMatrix * meshOffsets[meshIndex].localMatrix;

			//	If skeletal mesh
			if (mesh->getArchetype() == SkeletalMesh::staticGetArchetype())
			{
				//	Bind SKELETAL MESH vao
				glBindVertexArray(renderSystem.GPUWrapper.skeletalMeshVAO.GetID());
				const SkeletalMesh* skMesh = static_cast<const SkeletalMesh*>(mesh);


				int i = 0;
				for (const MeshData& subMesh : skMesh->subMeshes)
				{
					//	Try get the correct skeleton
					const SkeletalData* skeleton = i < skMesh->skeletonDatas.size() ? &skMesh->skeletonDatas[i] : nullptr;
					if (!skeleton) { i++; continue; }

					//	Get material and binded shader
					const Material* material = (matCount > 0 && materials[i % matCount]) ? materials[i % matCount] : nullptr;
					const MeshShader& shader = material ? GetMeshShader(material->shader) : renderSystem.litShader;

					//	Draw the skeletal mesh
					shader.BindSkeletalMeshShader();
					shader.DrawSkeletalMesh(worldModelMatrix, subMesh, *skeleton, material ? &material->data : nullptr);

					i++;
				}

				meshIndex++;

				//	Skip regular mesh drawing
				continue;
			}

			//	Else if it is a mesh

			//	Bind MESH vao
			glBindVertexArray(renderSystem.GPUWrapper.meshVAO.GetID());

			int i = 0;
			for (const MeshData& subMesh : mesh->subMeshes)
			{
				//	Get material and binded shader
				const Material* material = (matCount > 0 && materials[i % matCount]) ? materials[i % matCount] : nullptr;
				const MeshShader& shader = material ? GetMeshShader(material->shader) : renderSystem.litShader;

				//	Draw the mesh
				shader.BindMeshShader();
				shader.DrawMesh(worldModelMatrix, subMesh, material ? &material->data : nullptr);

				i++;
			}

			meshIndex++;
		}

		m_framebuffer.EndDraw();
		m_framebuffer.Unbind();
	}

	//	Make sure to unbind both vao and shader
	renderSystem.litShader.UnbindShader();
	glBindVertexArray(0);

	//	Clear the queue
	m_modelsToRenderPreview.clear();
}


void RenderPreview::Render()
{
	RenderSystem& renderSystem = SystemManager::GetRenderSystem();

	renderSystem.SendCameraUBO(m_camera);
	renderSystem.SendLightUBO(ligths);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	RenderMaterials();
	RenderMeshes();
	RenderSkeletalMeshes();
	RenderModels();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}



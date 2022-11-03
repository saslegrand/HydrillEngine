#include "EditorResourcePreview.hpp"

#include <glad/gl.h>

#include <EngineContext.hpp>
#include <Resources/Resource/Texture.hpp>
#include <Resources/Resource/Material.hpp>
#include <Resources/Resource/Mesh.hpp>
#include <Resources/Resource/SkeletalMesh.hpp>
#include <Resources/Resource/Model.hpp>
#include <Renderer/Primitives/GLTexture.hpp>
#include <Renderer/RenderSystem.hpp>

#include "PathConfig.hpp"


EditorResourcePreview::EditorResourcePreview()
{

}

EditorResourcePreview::~EditorResourcePreview()
{
}

void EditorResourcePreview::Initialize()
{
	m_renderPreview.Initialize();
}


void EditorResourcePreview::Update()
{
	m_renderPreview.Render();
}


unsigned int EditorResourcePreview::GenerateResourcePreview(Resource& resource)
{
	if (resource.getArchetype() == Texture::staticGetArchetype())
	{
		const Texture& text = static_cast<const Texture&>(resource);

		if (text.GPUData == nullptr || text.GPUData->generatedTexture == nullptr)
		{
			return 0;
		}

		return text.GPUData->generatedTexture->GetID();
	}

	return m_renderPreview.AddResourceInQueue(resource);
}


unsigned int EditorResourcePreview::GetResourcePreview(Resource& resource)
{
	if (m_resourcePreview.find(&resource) != m_resourcePreview.end())
	{
		return m_resourcePreview[&resource];
	}

	else
	{
		m_resourcePreview[&resource] = GenerateResourcePreview(resource);
	}

	return m_resourcePreview[&resource];

	return 0;
}

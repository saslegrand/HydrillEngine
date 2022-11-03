#include "EditorImages.hpp"

#include <glad/gl.h>

#include <EngineContext.hpp>
#include <Resources/ResourcesManager.hpp>
#include <Resources/Resource/Texture.hpp>
#include <Renderer/Primitives/GLTexture.hpp>
#include <Renderer/GPUTextureData.hpp>

#include "PathConfig.hpp"


EditorImages::EditorImages()
{

}

EditorImages::~EditorImages()
{
	for (auto& it : generatedImages)
	{
		glDeleteTextures(1, &it.second);
	}
}

unsigned int EditorImages::LoadImage(const char* path)
{
	if (generatedImages.find(path) != generatedImages.end()) return generatedImages[path];

	Texture* texture = static_cast<Texture*>(EngineContext::Instance().resourcesManager->GetResourceByPath(INTERNAL_EDITOR_RESOURCES_ROOT + std::string(path)));

	return texture != nullptr ? texture->GPUData->generatedTexture->GetID() : 0;
}


void EditorImages::UnloadImage(const char* path)
{
	Resource* r = EngineContext::Instance().resourcesManager->GetResourceByPath(path);
	if(r) EngineContext::Instance().resourcesManager->DeleteResource(r);

	if (generatedImages.find(path) != generatedImages.end()) glDeleteTextures(1, &generatedImages[path]);
}

unsigned int EditorImages::GetImageFromPath(const char* path)
{
	if (generatedImages.find(path) != generatedImages.end()) return generatedImages[path];

	return 0;
}
#include <glad/gl.h>

#include "ECS/Systems/SystemManager.hpp"
#include "Renderer/OpaqueRenderPipeline.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Renderer/ShaderType.hpp"
#include "Renderer/MeshInstance.hpp"
#include "Resources/Resource/Mesh.hpp"
#include "Resources/Resource/Material.hpp"
#include "Core/Logger.hpp"
//#include "ECS/MeshComponent.hpp"
//#include "ECS/SkeletalMeshComponent.hpp"
#include "Maths/Matrix4.hpp"
#include "EngineContext.hpp"


OpaqueRenderPipeline::OpaqueRenderPipeline()
{
}


OpaqueRenderPipeline::~OpaqueRenderPipeline()
{
}

void OpaqueRenderPipeline::Initialize()
{
    RenderSystem& render = SystemManager::GetRenderSystem();
    unlitShader = &render.unlitShader;
    litShader   = &render.litShader;
    pbrShader   = &render.pbrShader;
}

void OpaqueRenderPipeline::AddMeshInstance(MeshInstance* instance)
{
    if (instance->material)
    {
        switch (instance->material->shader)
        {
        case ShaderType::LIT:
            litShader->AddMeshInstance(instance);
            return;

        case ShaderType::PBR:
            pbrShader->AddMeshInstance(instance);
            return;

        default: break;
        }
    }
    unlitShader->AddMeshInstance(instance);
}

void OpaqueRenderPipeline::RemoveMeshInstance(MeshInstance* instance)
{
    if (instance->savedPipeline) instance->savedPipeline->RemoveMeshInstance(instance);

    unlitShader->RemoveMeshInstance(instance);
}

void OpaqueRenderPipeline::AddSkeletalMeshInstance(SkeletalMeshInstance* instance)
{
    if (instance->material)
    {
        switch (instance->material->shader)
        {
        case ShaderType::LIT:
            litShader->AddSkeletalMeshInstance(instance);
            return;

        case ShaderType::PBR:
            pbrShader->AddSkeletalMeshInstance(instance);
            return;

        default: break;
        }
    }

    unlitShader->AddSkeletalMeshInstance(instance);
}

void OpaqueRenderPipeline::RemoveSkeletalMeshInstance(SkeletalMeshInstance* instance)
{
    if(instance->savedPipeline) instance->savedPipeline->RemoveMeshInstance(instance);

    unlitShader->RemoveSkeletalMeshInstance(instance);
}


void OpaqueRenderPipeline::Render()
{
    unlitShader->Render();
    litShader->Render();
    pbrShader->Render();
}
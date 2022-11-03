#include "Renderer/TransparentRenderPipeline.hpp"

#include <glad/gl.h>
#include <algorithm>

#include "Renderer/RenderSystem.hpp"
#include "Renderer/RenderObjects/Camera.hpp"
#include "Renderer/ShaderType.hpp"
#include "Renderer/MeshInstance.hpp"

#include "Resources/Resource/Mesh.hpp"
#include "ECS/MeshComponent.hpp"
#include "ECS/SkeletalMeshComponent.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/GameObject.hpp"
#include "Maths/Matrix4.hpp"
#include "EngineContext.hpp"


TransparentRenderPipeline::TransparentRenderPipeline()
{
}


TransparentRenderPipeline::~TransparentRenderPipeline()
{
}

void TransparentRenderPipeline::Initialize()
{
    RenderSystem& render = SystemManager::GetRenderSystem();
    unlitShader = &render.unlitShader;
    litShader = &render.litShader;
    pbrShader = &render.pbrShader;
}

void TransparentRenderPipeline::AddMeshInstance(MeshInstance* instance)
{
    if (instance == nullptr) return;

    auto foundComp = std::find(meshes.begin(), meshes.end(), instance);

    if (foundComp != meshes.end())
    {
        Logger::Warning("MeshRenderPipeline - Can't register one mesh component instance multiple times");
        return;
    }

    instance->savedPipeline = this;
    meshes.emplace_back(instance);
}

void TransparentRenderPipeline::RemoveMeshInstance(MeshInstance* instance)
{
    if (instance == nullptr || meshes.empty()) return;

    auto foundComp = std::find(meshes.begin(), meshes.end(), instance);

    if (foundComp == meshes.end())
    {
        return;
    }

    instance->savedPipeline = nullptr;
    meshes.erase(foundComp);
}

void TransparentRenderPipeline::AddSkeletalMeshInstance(SkeletalMeshInstance* instance)
{
    if (instance == nullptr) return;

    auto foundComp = std::find(skeletalMeshes.begin(), skeletalMeshes.end(), instance);

    if (foundComp != skeletalMeshes.end())
    {
        Logger::Warning("MeshRenderPipeline - Can't register one mesh component instance multiple times");
        return;
    }

    instance->savedPipeline = this;
    skeletalMeshes.emplace_back(instance);
}

void TransparentRenderPipeline::RemoveSkeletalMeshInstance(SkeletalMeshInstance* instance)
{
    if (instance == nullptr || skeletalMeshes.empty()) return;

    auto foundComp = std::find(skeletalMeshes.begin(), skeletalMeshes.end(), instance);

    if (foundComp == skeletalMeshes.end())
    {
        return;
    }

    instance->savedPipeline = nullptr;
    skeletalMeshes.erase(foundComp);
}


std::vector<TransparentRenderPipeline::BlendMesh> TransparentRenderPipeline::SortMeshes()
{
    std::vector<BlendMesh> out;
    Vector3 camPos = SystemManager::GetCameraSystem().renderingCamera->GetPosition();

    for (const MeshInstance* instance : meshes)
    {
        float dist = (instance->transform->Position() - camPos).SquaredMagnitude();

        out.emplace_back(dist, instance, false);
    }

    for (const SkeletalMeshInstance* instance : skeletalMeshes)
    {
        float dist = (instance->transform->Position() - camPos).SquaredMagnitude();

        out.emplace_back(dist, instance, true);

    }

    std::sort(out.begin(), out.end(), [](const BlendMesh& a, const BlendMesh& b)->bool
        {
            return a.distanceFromCamera > b.distanceFromCamera;
        }
    );

    return out;
}


void TransparentRenderPipeline::Render()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::vector<BlendMesh> sortedMeshes = SortMeshes();

    for (const BlendMesh& packedMesh : sortedMeshes)
    {
        if (packedMesh.skeletal)
        {
            const SkeletalMeshInstance* skInstance = static_cast<const SkeletalMeshInstance*>(packedMesh.instance);

            switch (packedMesh.instance->material->shader)
            {
            case ShaderType::UNLIT:
                unlitShader->RenderSingleInstance(skInstance);
                break;

            case ShaderType::LIT:
                litShader->RenderSingleInstance(skInstance);
                break;

            case ShaderType::PBR:
                pbrShader->RenderSingleInstance(skInstance);
                break;

            default: break;
            }
        }
        else
        {
            switch (packedMesh.instance->material->shader)
            {
            case ShaderType::UNLIT:
                unlitShader->RenderSingleInstance(packedMesh.instance);
                break;

            case ShaderType::LIT:
                litShader->RenderSingleInstance(packedMesh.instance);
                break;

            case ShaderType::PBR:
                pbrShader->RenderSingleInstance(packedMesh.instance);
                break;

            default: break;
            }
        }
    }

    glDisable(GL_BLEND);
}
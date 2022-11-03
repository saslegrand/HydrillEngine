#include "Renderer/MeshShader.hpp"

#include <glad/gl.h>

#include "Core/Logger.hpp"
#include "Renderer/RenderSystem.hpp"
#include "ECS/Systems/SystemManager.hpp"
#include "Renderer/MeshInstance.hpp"
#include "Resources/Resource/Mesh.hpp"
#include "Resources/Resource/Material.hpp"
#include "EngineContext.hpp"


MeshShader::MeshShader()
{
}

MeshShader::~MeshShader()
{

}

void MeshShader::BindMeshShader() const
{
    m_meshShader.Bind();
}

void MeshShader::BindSkeletalMeshShader() const
{
    m_skeletalMeshShader.Bind();
}

void MeshShader::UnbindShader() const
{
    glUseProgram(0);
}


void MeshShader::ClearMeshInstances()
{
    m_meshInstances.clear();
}


void MeshShader::AddMeshInstance(MeshInstance* instance)
{
    if (instance == nullptr) return;

    auto foundComp = std::find(m_meshInstances.begin(), m_meshInstances.end(), instance);

    if (foundComp != m_meshInstances.end())
    {
        Logger::Warning("MeshShader - Can't register one mesh component instance multiple times");
        return;
    }

    instance->savedPipeline = this;
    m_meshInstances.emplace_back(instance);
}


void MeshShader::RemoveMeshInstance(MeshInstance* instance)
{
    if (instance == nullptr || m_meshInstances.empty()) return;

    auto foundComp = std::find(m_meshInstances.begin(), m_meshInstances.end(), instance);

    if (foundComp == m_meshInstances.end())
    {
        return;
    }

    instance->savedPipeline = nullptr;
    m_meshInstances.erase(foundComp);
}


void MeshShader::AddSkeletalMeshInstance(SkeletalMeshInstance* instance)
{
    auto foundComp = std::find(m_skeletalMeshInstances.begin(), m_skeletalMeshInstances.end(), instance);

    if (foundComp != m_skeletalMeshInstances.end())
    {
        Logger::Warning("MeshShader - Can't register mesh components multiple times");
        return;
    }

    instance->savedPipeline = this;
    m_skeletalMeshInstances.emplace_back(instance);
}


void MeshShader::RemoveSkeletalMeshInstance(SkeletalMeshInstance* instance)
{
    auto foundComp = std::find(m_skeletalMeshInstances.begin(), m_skeletalMeshInstances.end(), instance);

    if (foundComp == m_skeletalMeshInstances.end())
    {
        Logger::Error("MeshShader - Failed to unregister a non-existing mesh component");
        return;
    }

    instance->savedPipeline = nullptr;
    m_skeletalMeshInstances.erase(foundComp);
}

void MeshShader::RenderSingleInstance(const MeshInstance* instance) const
{
    if (!instance->mesh) return;

    Matrix4 model = instance->transform ? instance->transform->GetWorldMatrix() : Matrix4::Identity;
    MeshData& mesh = *instance->mesh;
    MaterialData* mat = instance->material ? &instance->material->data : nullptr;

    RenderSystem& render = SystemManager::GetRenderSystem();

    BindMeshShader();
    render.GPUWrapper.meshVAO.Bind();
    
    DrawMesh(model, mesh, mat);

    render.GPUWrapper.meshVAO.Unbind();
    UnbindShader();
}


void MeshShader::RenderSingleInstance(const SkeletalMeshInstance* instance) const
{
    if (!instance->mesh || !instance->skeleton) return;

    Matrix4 model = instance->transform ? instance->transform->GetWorldMatrix() : Matrix4::Identity;
    MeshData& mesh = *instance->mesh;
    SkeletalData& skeleton = *instance->skeleton;
    MaterialData* mat = instance->material ? &instance->material->data : nullptr;

    RenderSystem& render = SystemManager::GetRenderSystem();

    BindSkeletalMeshShader();
    render.GPUWrapper.skeletalMeshVAO.Bind();

    DrawSkeletalMesh(model, mesh, skeleton, mat);

    render.GPUWrapper.skeletalMeshVAO.Unbind();
    UnbindShader();
}

void MeshShader::Render()
{
    RenderSystem& render = SystemManager::GetRenderSystem();

    m_meshShader.Bind();
    render.GPUWrapper.meshVAO.Bind();

    for (const MeshInstance* instance : m_meshInstances)
    {
        if (!instance->mesh || !instance->isActive) continue;

        Matrix4 model = instance->transform ? instance->transform->GetWorldMatrix() : Matrix4::Identity;
        MeshData& mesh = *instance->mesh;
        MaterialData* mat = instance->material ? &instance->material->data : nullptr;

        DrawMesh(model, mesh, mat);
    }

    m_skeletalMeshShader.Bind();
    render.GPUWrapper.skeletalMeshVAO.Bind();

    for (const SkeletalMeshInstance* instance : m_skeletalMeshInstances)
    {
        if (!instance->mesh || !instance->isActive) continue;

        Matrix4 model = instance->transform ? instance->transform->GetWorldMatrix() : Matrix4::Identity;
        MeshData& mesh = *instance->mesh;
        SkeletalData& skeleton = *instance->skeleton;
        MaterialData* mat = instance->material ? &instance->material->data : nullptr;

        DrawSkeletalMesh(model, mesh, skeleton, mat);
    }

    render.GPUWrapper.skeletalMeshVAO.Unbind();
    m_skeletalMeshShader.Unbind();
}

const MaterialData* MeshShader::m_defaultMat = nullptr;

void MeshShader::SetDefaultMaterial(const MaterialData* material)
{
    MeshShader::m_defaultMat = material;
}

#include "Renderer/SkyboxRenderPipeline.hpp"

#include <glad/gl.h>

#include "EngineContext.hpp"
#include "Maths/Matrix3.hpp"

#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/SystemManager.hpp"
#include "Renderer/Primitives/GLCubeMap.hpp"
#include "Renderer/RenderObjects/Camera.hpp"
#include "Renderer/GPUMeshData.hpp"

#include "Resources/ResourcesManager.hpp"
#include "Resources/Resource/Skybox.hpp"
#include "Resources/Resource/Mesh.hpp"

#pragma region LIT SHADER REGION

static const char* skyboxVertexShaderStr = R"GLSL(
#version 450 core

layout (location = 0) in vec3 aPosition;

out vec3 TexCoords;

uniform mat4 uViewProj;

void main()
{
    TexCoords = aPosition;
    vec4 pos = uViewProj * vec4(aPosition, 1.0);
    gl_Position = pos.xyww;
})GLSL";

static const char* skyboxFragmentShaderStr = R"GLSL(
#version 450 core

out vec4 FragColor;
in  vec3 TexCoords;

uniform samplerCube uSkyTexture;

void main()
{   
    vec3 toLinear = vec3(2.2);
    FragColor = vec4(pow(texture(uSkyTexture, TexCoords).rgb, toLinear),1.0);
})GLSL";

#pragma endregion


SkyboxRenderPipeline::SkyboxRenderPipeline()
{
    
}

SkyboxRenderPipeline::~SkyboxRenderPipeline()
{
}

void SkyboxRenderPipeline::Initialize()
{
    int i = 0;

    shader.Generate(skyboxVertexShaderStr, skyboxFragmentShaderStr);

    shader.Bind();
    shader.SendUniform("uSkyTexture", &i);
    shader.Unbind();

    Resource* res = EngineContext::Instance().resourcesManager->GetResourceByFilename("CubeInternal.mesh");
    if (res)
    {
        cubeMesh = static_cast<Mesh*>(res);
        InitCube();
    }
}

void SkyboxRenderPipeline::InitCube()
{
        //	Meshes Vertex Array
    GLuint vao = cubeVAO.GetID();

    // Allocate vertex buffer storage (empty)
    MeshData const& data = cubeMesh->subMeshes[0];
    glNamedBufferStorage(cubeVBO.GetID(), data.GPUData->size, data.vertices.data(), GL_DYNAMIC_STORAGE_BIT);

    // Link array and buffer (binding 0)
    glVertexArrayVertexBuffer(vao, 0, cubeVBO.GetID(), 0, sizeof(Vertex));

    // Enable attributes
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
}

void SkyboxRenderPipeline::Render()
{
    if (attachedSkybox == nullptr)
        return;

    if (cubeMesh == nullptr)
    {
        Resource* res = EngineContext::Instance().resourcesManager->GetResourceByFilename("CubeInternal.mesh");
        if (!res)
            return;

        cubeMesh = static_cast<Mesh*>(res);
        InitCube();
    }

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

    const Camera* camera = SystemManager::GetCameraSystem().renderingCamera;

    Matrix4 castView = Matrix3::ToMatrix4(Matrix4::ToMatrix3(camera->GetViewMatrix()));
    Matrix4 proj = camera->GetProjectionMatrix() * castView;

    shader.Bind();

    cubeVAO.Bind();

    // TODO Check for transpose
    shader.SendUniform("uViewProj", proj.elements);

    glActiveTexture(GL_TEXTURE0);
    attachedSkybox->GPUData->Bind();

    glDrawElementsBaseVertex(
        GL_TRIANGLES,
        static_cast<GLsizei>(cubeMesh->subMeshes[0].indices.size()),
        GL_UNSIGNED_INT,
        cubeMesh->subMeshes[0].indices.data(),
        0
    );

    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);

    cubeVAO.Unbind();

    shader.Unbind();
}

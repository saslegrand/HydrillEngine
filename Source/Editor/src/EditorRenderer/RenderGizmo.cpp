#include "EditorRenderer/RenderGizmo.hpp"

#include <glad/gl.h>

#include <EngineContext.hpp>
#include <Renderer/RenderSystem.hpp>
#include <ECS/Systems/CameraSystem.hpp>
#include <ECS/Systems/ParticleSystem.hpp>
#include <ECS/Systems/SoundSystem.hpp>
#include <ECS/GameObject.hpp>
#include <ECS/LightComponent.hpp>
#include <ECS/ParticleComponent.hpp>
#include <ECS/CameraComponent.hpp>
#include <ECS/Sound/SoundEmitter.hpp>
#include <Tools/DrawDebug.hpp>

#include "EditorImages.hpp"
#include "EditorContext.hpp"
#include "EditorTheme.hpp"
#include "EditorSelectionManager.hpp"

static const char* gizmoVertexShaderStr = R"GLSL(
#version 450 core

// Attributes

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;

//  Uniform Buffers

layout (std140, binding = 0) uniform UBO_matrices
{
    mat4 uProjection;
    mat4 uView;
};

// Uniforms

uniform mat4 uModel;

//  Varyings

out vec2 vUV;

//  Functions

void main()
{
    vUV = aUV;
    gl_Position = uProjection * uView * uModel * vec4(aPosition.xyz, 1.0);

})GLSL";


static const char* gizmoFragmentShaderStr = R"GLSL(
#version 450 core

// Uniforms

uniform sampler2D uTexture;
uniform vec4 uColor;

//  Shader Inputs

in vec2 vUV;

//  Shader Outputs

out vec4 oColor;

//  Functions

void main()
{
    vec4 fragment = texture(uTexture, vUV);
    oColor = uColor * fragment;
})GLSL";

void RenderGizmo::DrawGizmoSprite(const Transform& transform, unsigned int texture, const Color4& color)
{
    Matrix4 model = Matrix4::Translate(transform.Position()) * m_billboardRotationMatrix * Matrix4::Scale(Vector3::One);

    glBindTextureUnit(0, texture);
    m_shader.SendUniform("uModel", model.elements);
    m_shader.SendUniform("uColor", &color.r);

    glDrawArrays(GL_TRIANGLES,0,6);
}

void RenderGizmo::Initialize()
{
    m_shader.Generate(gizmoVertexShaderStr, gizmoFragmentShaderStr);
    
    m_shader.Bind();
    glUniform1i(glGetUniformLocation(m_shader.GetID(), "uTexture"), 0);
    m_shader.Unbind();

    GLuint vao = m_rectVAO.GetID();

    {
        // Enable attributes
        glEnableVertexArrayAttrib(vao, 0);
        glEnableVertexArrayAttrib(vao, 1);

        // Bind vao attributes to the vbo (from binding index)
        glVertexArrayAttribBinding(vao, 0, 0);
        glVertexArrayAttribBinding(vao, 1, 0);

        // Set attributes format
        glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
    }

    {
        float vertices[] = {
            //    X      Y     Z         U     V
                -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,
                 0.5f, -0.5f, 0.0f,     1.0f, 0.0f,
                 0.5f,  0.5f, 0.0f,     1.0f, 1.0f,
                 0.5f,  0.5f, 0.0f,     1.0f, 1.0f,
                -0.5f,  0.5f, 0.0f,     0.0f, 1.0f,
                -0.5f, -0.5f, 0.0f,     0.0f, 0.0f
        };

        VertexBuffer VBO;

        GLuint vbo = VBO.GetID();

        // Allocate vertex buffer storage (empty)
        glNamedBufferStorage(vbo, sizeof(vertices), vertices, GL_MAP_READ_BIT);

        // Link array and buffer (binding 0)
        glVertexArrayVertexBuffer(vao, 0, vbo, 0, 5 * sizeof(float));
    }

    EditorImages* img = EditorContext::Instance().images;

    m_lightGizmoTexture         = img->LoadImage(R"(Icons\LightGizmo.texture)");
    m_cameraGizmoTexture        = img->LoadImage(R"(Icons\CameraGizmo.texture)");
    m_particleGizmoTexture      = img->LoadImage(R"(Icons\ParticleGizmo.texture)");
    m_soundEmitterGizmoTexture  = img->LoadImage(R"(Icons\SoundEmitterGizmo.texture)");
}


void RenderGizmo::Render()
{
    EngineContext& engineContext = EngineContext::Instance();
    EditorContext& editorContext = EditorContext::Instance();

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    m_rectVAO.Bind();
    m_shader.Bind();

    m_billboardRotationMatrix = Quaternion::ToMatrix(SystemManager::GetCameraSystem().renderingCamera->GetBillboardRotation());

    for (auto& light : SystemManager::GetRenderSystem().GetAllLights())
    {
        if (editorContext.selectionManager->IsSelected(light->gameObject))
        {
            switch(light->type)
            {
            case LightType::Point :
                DrawDebug::DrawSphere(light->gameObject.transform.GetWorldMatrix(), light->radius, 2.0f, editorContext.theme->color_interactives.defaultColor);
                break;

            case LightType::Spot:
                DrawDebug::DrawCone(light->gameObject.transform.GetWorldMatrix() * Matrix4::RotateX(Maths::PIHalf),
                    Maths::Sin(light->outerCutoff) * light->radius,
                    light->radius, 2.f, editorContext.theme->color_interactives.defaultColor);
                break;

            default :
                DrawDebug::DrawLine(light->gameObject.transform.Position(), light->gameObject.transform.Position() + light->gameObject.transform.Forward(), 5.f, Color3::Red);
                break;
            }
        }

        DrawGizmoSprite(light->gameObject.transform, m_lightGizmoTexture, Color4(light->diffuse, 0.75f));
    }

    selectedCamera = nullptr;
    for (auto& camera : SystemManager::GetCameraSystem().GetAllCameraComponents())
    {
        if (editorContext.selectionManager->IsSelected(camera->gameObject))
        {
            selectedCamera = &camera->camera; 
            camera->UpdateCamera();

            DrawDebug::DrawBox(Matrix4::Inverse(selectedCamera->GetProjectionMatrix() * selectedCamera->GetViewMatrix()), 1.f, 1.f,1.f,2.f, editorContext.theme->color_interactives.defaultColor);
        }
        DrawGizmoSprite(camera->gameObject.transform, m_cameraGizmoTexture);
    }

    for (auto& particle : SystemManager::GetParticleSystem().GetAllParticleComponents())
    {
        DrawGizmoSprite(particle->gameObject.transform, m_particleGizmoTexture);
    }

    for (auto& soundEmitter : SystemManager::GetSoundSystem().GetAllSoundEmitters())
    {
        DrawGizmoSprite(soundEmitter->gameObject.transform, m_soundEmitterGizmoTexture);
    }

    m_shader.Unbind();
    m_rectVAO.Unbind();
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
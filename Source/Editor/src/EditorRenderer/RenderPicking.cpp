#include "EditorRenderer/RenderPicking.hpp"
#include "ECS/GameObject.hpp"

#include <glad/gl.h>

#include <Resources/Resource/Mesh.hpp>
#include <Resources/Resource/SkeletalMesh.hpp>

#include <Renderer/GPUMeshData.hpp>
#include <Renderer/RenderSystem.hpp>
#include <Renderer/MeshInstance.hpp>

#include <ECS/Systems/CameraSystem.hpp>
#include <ECS/Systems/ParticleSystem.hpp>
#include <ECS/Systems/SoundSystem.hpp>
#include <ECS/Transform.hpp>
#include <ECS/GameObject.hpp>
#include <ECS/LightComponent.hpp>
#include <ECS/CameraComponent.hpp>
#include <ECS/ParticleComponent.hpp>
#include <ECS/Sound/SoundEmitter.hpp>

#include <IO/Mouse.hpp>
#include <IO/Keyboard.hpp>
#include <Types/Color.hpp>
#include <EngineContext.hpp>

#include "EditorContext.hpp"
#include "EditorSelectionManager.hpp"

#pragma region PICKING SHADER REGION

static const char* pickingVertexShaderStr = R"GLSL(
#version 450 core

// Attributes

layout(location = 0) in vec3 aPosition;

//  Uniform Buffers

layout (std140, binding = 0) uniform UBO_matrices
{
    mat4 uProjection;
    mat4 uView;
};

// Uniforms

uniform mat4 uModel;

//  Functions

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);

})GLSL";

static const char* skeletalPickingVertexShaderStr = R"GLSL(
#version 450 core

// Attributes

layout(location = 0) in vec3 aPosition;
//  1,2,3 ... unused locations
layout(location = 4) in vec4 aBoneIndices;
layout(location = 5) in vec4 aBoneWeights;

//  Uniform Buffers

layout (std140, binding = 0) uniform UBO_matrices
{
    mat4 uProjection;
    mat4 uView;
};

// Uniforms

uniform mat4 uModel;

uniform mat4 uSkinningMatrices[64];

//  Functions

void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < 4 ; i++)
    {
        if(aBoneIndices[i] == -1) 
            continue;
        if(aBoneIndices[i] >= 100) 
        {
            totalPosition = vec4(aPosition, 1.0f);
            break;
        }
        
        vec4 localPosition = uSkinningMatrices[int(aBoneIndices[i])] * vec4(aPosition, 1.0f);
        totalPosition += localPosition * aBoneWeights[i];
    }

    gl_Position = uProjection * uView * uModel * vec4(totalPosition.xyz, 1.0);

})GLSL";


static const char* pickingFragmentShaderStr = R"GLSL(
#version 450 core

// Uniforms

uniform vec4 uPickingColor;

//  Shader Output

out vec4 oColor;

//  Functions

void main()
{
    oColor = uPickingColor;
})GLSL";

#pragma endregion

void RenderPicking::Initialize()
{
    //  Load Shader
    m_meshShader.Generate(pickingVertexShaderStr, pickingFragmentShaderStr);
    m_skeletalMeshShader.Generate(skeletalPickingVertexShaderStr, pickingFragmentShaderStr);

    // Create the FBO
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    //  Texture Generation
    glGenTextures(1, &m_pickingTexture);
    glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    //  Bind FrameBuffer to Texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingTexture, 0);

    //  Render buffer generation

    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);


    //  Bind FrameBuffer to Render buffer 
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

    // Disable reading to avoid problems with older GPUs
    glReadBuffer(GL_NONE);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // Verify that the FBO is correct
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
}


void RenderPicking::DrawMesh(const MeshInstance* instance)
{
    m_meshShader.SendUniform("uModel", instance->transform->GetWorldMatrix().elements);

    glDrawElementsBaseVertex(
        GL_TRIANGLES,
        static_cast<GLsizei>(instance->mesh->indices.size()),
        GL_UNSIGNED_INT,
        instance->mesh->indices.data(),
        instance->mesh->GPUData->offset
    );
}


void RenderPicking::DrawSkeletalMesh(const SkeletalMeshInstance* instance)
{
    m_skeletalMeshShader.SendUniform("uModel", instance->transform->GetWorldMatrix().elements);
    m_skeletalMeshShader.SendUniform("uSkinningMatrices[0]", instance->skeleton->boneTransforms.data(), static_cast<int>(instance->skeleton->boneTransforms.size()));

    glDrawElementsBaseVertex(
        GL_TRIANGLES,
        static_cast<GLsizei>(instance->mesh->indices.size()),
        GL_UNSIGNED_INT,
        instance->mesh->indices.data(),
        instance->mesh->GPUData->offset
    );
}

template<DerivedComponent comp>
void AddGizmoObjectsFromList(std::vector<const GameObject*>& dst, const std::vector<std::unique_ptr<comp>>& src)
{
    for (auto& component : src)
    {
        dst.emplace_back(&component->gameObject);
    }
}

void RenderPicking::GenerateGizmoObjects()
{
    m_gizmoGameObjects.clear();

    AddGizmoObjectsFromList(m_gizmoGameObjects, SystemManager::GetRenderSystem().GetAllLights());
    AddGizmoObjectsFromList(m_gizmoGameObjects, SystemManager::GetCameraSystem().GetAllCameraComponents());
    AddGizmoObjectsFromList(m_gizmoGameObjects, SystemManager::GetParticleSystem().GetAllParticleComponents());
    AddGizmoObjectsFromList(m_gizmoGameObjects, SystemManager::GetSoundSystem().GetAllSoundEmitters());
}

void RenderPicking::UpdateDimensions(Vector2 newDimensions)
{
    m_dimensions = newDimensions;

    glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, static_cast<GLsizei>(m_dimensions.x), static_cast<GLsizei>(m_dimensions.y), 0, GL_RGB, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(m_dimensions.x), static_cast<GLsizei>(m_dimensions.y));
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


void RenderPicking::UpdatePosition(Vector2 newPosition)
{
    m_position = newPosition;
}


void RenderPicking::RenderIDs()
{
    RenderSystem& renderSystem = SystemManager::GetRenderSystem();

    m_meshShader.Bind();
    renderSystem.GPUWrapper.meshVAO.Bind();

    int i = 0;

    for (const MeshInstance* instance : renderSystem.GetAllMeshInstances())
    {
        Color4 colorID = {
            (i & 0x000000FF) >> 0,
            (i & 0x0000FF00) >> 8,
            (i & 0x00FF0000) >> 16
        };

        m_meshShader.SendUniform("uPickingColor", &colorID);

        DrawMesh(instance);

        i++;
    }

    m_gizmoOffsetID = i;

    Matrix4 billboardRotationMatrix = Quaternion::ToMatrix(SystemManager::GetCameraSystem().renderingCamera->GetBillboardRotation());

    m_rectVAO.Bind();
    for (const GameObject* instance : m_gizmoGameObjects)
    {
        Color4 colorID = {
            (i & 0x000000FF) >> 0,
            (i & 0x0000FF00) >> 8,
            (i & 0x00FF0000) >> 16
        };

        m_meshShader.SendUniform("uPickingColor", &colorID);

        Matrix4 model = Matrix4::Translate(instance->transform.Position()) * billboardRotationMatrix * Matrix4::Scale(Vector3::One);

        m_meshShader.SendUniform("uModel", model.elements);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        i++;
    }
    m_rectVAO.Unbind();

    m_skeletalOffsetID = i;

    m_skeletalMeshShader.Bind();
    renderSystem.GPUWrapper.skeletalMeshVAO.Bind();

    for (const SkeletalMeshInstance* instance : renderSystem.GetAllSkeletalMeshInstances())
    {

        Color4 colorID = {
            (i & 0x000000FF) >> 0,
            (i & 0x0000FF00) >> 8,
            (i & 0x00FF0000) >> 16
        };

        m_meshShader.SendUniform("uPickingColor", &colorID);

        DrawSkeletalMesh(instance);

        i++;
    }

    m_skeletalMeshShader.Unbind();

    renderSystem.GPUWrapper.skeletalMeshVAO.Unbind();
}

enum ClickState
{
    LEFTCLICK_NONE,

    LEFTCLICK_PRESSED,
    LEFTCLICK_UNPRESSED
};
 
void RenderPicking::Render()
{
    updated = false;

    Vector2 mousePos = Input::Mouse::GetPosition();
    Vector2 localMousePos = mousePos - m_position;
    
    if ((localMousePos.x >= 0.0f && localMousePos.x <= m_dimensions.x) &&
        (localMousePos.y >= 0.0f && localMousePos.y <= m_dimensions.y))
    {

        int state = LEFTCLICK_NONE;
        bool shiftDown = Input::Keyboard::GetKeyDown(Key::LeftShift) || Input::Keyboard::GetKeyDown(Key::RightShift);
        bool ctrlDown = Input::Keyboard::GetKeyDown(Key::LeftControl) || Input::Keyboard::GetKeyDown(Key::RightControl);

        if (Input::Mouse::GetButtonPressed(MouseButton::Left)) state = LEFTCLICK_PRESSED;
        else if (Input::Mouse::GetButtonUnpressed(MouseButton::Left)) state = LEFTCLICK_UNPRESSED;

        GLint previousDrawingFramebuffer = 0;
        GLint previousReadingFramebuffer = 0;

        if (state == LEFTCLICK_NONE)return;

        GenerateGizmoObjects();

        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousDrawingFramebuffer);
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &previousReadingFramebuffer);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);

        glViewport(0, 0, static_cast<GLsizei>(m_dimensions.x), static_cast<GLsizei>(m_dimensions.y));
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //  Inverse y
        localMousePos.y = Maths::Abs(m_dimensions.y - localMousePos.y);

        RenderIDs();

        glFlush();
        glFinish();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, previousDrawingFramebuffer);

        unsigned char data[4];

        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
        glReadBuffer(GL_COLOR_ATTACHMENT0);

        //  "MousePos - ViewportPos" to remove padding caused by the editor interface
        glReadPixels(static_cast<int>(localMousePos.x), static_cast<int>(localMousePos.y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, previousReadingFramebuffer);

        selectedColor = Color4(data[0], data[1], data[2]);

        int pickedID =
            data[0] +
            data[1] * 256 +
            data[2] * 256 * 256;

        RenderSystem& renderSystem = SystemManager::GetRenderSystem();

        auto allMeshes = renderSystem.GetAllMeshInstances();
        auto allSkMeshes = renderSystem.GetAllSkeletalMeshInstances();

        size_t id = static_cast<size_t>(pickedID);
        size_t meshjIndex    = id;
        size_t gizmoObjIndex = id - static_cast<size_t>(m_gizmoOffsetID);
        size_t skMeshIndex   = id - static_cast<size_t>(m_skeletalOffsetID);

        //  if pressed
        if (state == LEFTCLICK_PRESSED)
        {
            if (meshjIndex < allMeshes.size())
            {
                m_pressedObject = &allMeshes[meshjIndex]->transform->gameObject;
            }
            else if (gizmoObjIndex < static_cast<int>(m_gizmoGameObjects.size()))
            {
                m_pressedObject = m_gizmoGameObjects[gizmoObjIndex];
            }
            else if (skMeshIndex < static_cast<int>(allSkMeshes.size()))
            {
                m_pressedObject = &allSkMeshes[skMeshIndex]->transform->gameObject;
            }
            else
            {
                m_pressedObject = nullptr;
            }

            m_wasPressed = true;
        }

        //else if unpressed
        else if (state == LEFTCLICK_UNPRESSED)
        {
            if (m_wasPressed)
            {
                bool meshSelected     = meshjIndex < allMeshes.size()             && m_pressedObject == &allMeshes[meshjIndex]->transform->gameObject;
                bool gizmoObjSelected = gizmoObjIndex < m_gizmoGameObjects.size() && m_pressedObject == m_gizmoGameObjects[gizmoObjIndex];
                bool skMeshSelected   = skMeshIndex < allSkMeshes.size()          && m_pressedObject == &allSkMeshes[skMeshIndex]->transform->gameObject;

                if (m_pressedObject && (meshSelected || skMeshSelected || gizmoObjSelected))
                {
                    if (!shiftDown && !ctrlDown)
                    {
                        EditorContext::Instance().selectionManager->ClearGameObjects();
                    }

                    if (ctrlDown &&  EditorContext::Instance().selectionManager->IsSelected(*m_pressedObject))
                    {
                        EditorContext::Instance().selectionManager->RemoveFromSelection(*m_pressedObject);
                    }
                    else
                    {
                        EditorContext::Instance().selectionManager->AddToSelection(*m_pressedObject);
                    }

                    m_pressedObject = nullptr;
                }
                else
                {
                    EditorContext::Instance().selectionManager->ClearGameObjects();
                }

                m_wasPressed = false;
                updated = true;
            }
        }
    }
    else if (m_wasPressed)
    {
        m_wasPressed = false;
    }
}
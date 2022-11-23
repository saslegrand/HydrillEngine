#include "Renderer/ShadowProcess.hpp"

#include "ECS/LightComponent.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Renderer/MeshInstance.hpp"
#include "Resources/Resource/SkeletalMesh.hpp"
#include "ECS/Systems/SystemManager.hpp"
#include "Core/Logger.hpp"

#pragma region SHADOWMAP SHADER REGION

static const char* meshShadowMapVertexShaderStr = R"GLSL(
#version 450 core

// Attributes
layout(location = 0) in vec3 aPosition;

// Uniforms
uniform mat4 uSpotLightMatrix;

void main()
{
    gl_Position = uSpotLightMatrix * vec4(aPosition, 1.0);
})GLSL";

static const char* skeletalMeshShadowMapVertexShaderStr = R"GLSL(
#version 450 core

// Attributes
layout(location = 0) in vec3 aPosition;

layout(location = 4) in vec4 aBoneIndices;
layout(location = 5) in vec4 aBoneWeights;

// Uniforms
uniform mat4 uSpotLightMatrix;
uniform mat4 uSkinningMatrices[128];

void main()
{
    vec3 totalPosition = vec3(1.0f);

    for(int i = 0 ; i < 4 ; i++)
    {
        if(aBoneIndices[i] == -1) continue;
        if(aBoneIndices[i] >= 128) 
        {
            totalPosition = aPosition;
            break;
        }
        vec4 localPosition = uSkinningMatrices[int(aBoneIndices[i])] * vec4(aPosition, 1.0f);
        totalPosition += (localPosition * aBoneWeights[i]).rgb;
    }

    gl_Position = uSpotLightMatrix * vec4(totalPosition,1.0);
})GLSL";


static const char* shadowMapFragmentShaderStr = R"GLSL(
#version 450 core

layout(location = 0) out float aFragDepth;

void main()
{
})GLSL";
#pragma endregion

#pragma region SHADOWCUBEMAP SHADER REGION

static const char* meshShadowCubeMapVertexShaderStr = R"GLSL(
#version 450 core

// Attributes
layout(location = 0) in vec3 aPosition;

// Uniforms

uniform mat4 uModel;

void main()
{
    gl_Position = uModel * vec4(aPosition, 1.0);
})GLSL";

static const char* skeletalMeshShadowCubeMapVertexShaderStr = R"GLSL(
#version 450 core

// Attributes
layout(location = 0) in vec3 aPosition;

layout(location = 4) in vec4 aBoneIndices;
layout(location = 5) in vec4 aBoneWeights;

// Uniforms

uniform mat4 uModel;
uniform mat4 uSkinningMatrices[128];

void main()
{
    vec3 totalPosition = vec3(1.0f);

    for(int i = 0 ; i < 4 ; i++)
    {
        if(aBoneIndices[i] == -1) continue;
        if(aBoneIndices[i] >= 128) 
        {
            totalPosition = aPosition;
            break;
        }
        vec4 localPosition = uSkinningMatrices[int(aBoneIndices[i])] * vec4(aPosition, 1.0f);
        totalPosition += (localPosition * aBoneWeights[i]).rgb;
    }

    gl_Position = uModel * vec4(totalPosition,1.0);
})GLSL";

static const char* shadowCubeMapGeometryShaderStr = R"GLSL(
#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

//  Uniforms
uniform mat4 uPointLightMatrix[6];
uniform int uIndex;

//  Shader Outputs
out vec4 oPosition;

void main()
{
    int offset = uIndex*6;

    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face + offset; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle vertex
        {
            oPosition = gl_in[i].gl_Position;
            gl_Position = uPointLightMatrix[face] * oPosition;
            EmitVertex();
        }    
        EndPrimitive();
    }
})GLSL";

static const char* shadowCubeMapFragmentShaderStr = R"GLSL(
#version 450 core

//  Shader Inputs
in vec4 oPosition;

//  Structures

struct Light
{
    vec3 position;
    vec3 direction;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
    vec2 cutOff;

    float radius;
    float intensity;

    int  type;

    bool castShadows;
    bool enabled;
};

//  Uniforms

uniform vec3  uPosition;
uniform int uIndex;

layout (std140, binding = 1) uniform UBO_Lights
{
    Light uLights[8];
    int   uNumLights;
};


void main()
{
    // get distance between fragment and light source
    float lightDistance = length(oPosition.xyz - uPosition);

    // map to [0;1] range by dividing by radius

    Light light = uLights[uIndex];
    lightDistance = lightDistance / light.radius;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
})GLSL";
#pragma endregion


#pragma region CASCADE SHADOWMAP SHADER REGION

static const char* meshCascadeShadowMapVertexShaderStr = R"GLSL(
#version 450 core

// Attributes
layout(location = 0) in vec3 aPosition;

// Uniforms

uniform mat4 uModel;

void main()
{
    gl_Position = uModel * vec4(aPosition, 1.0);
})GLSL";

static const char* skeletalMeshCascadeShadowMapVertexShaderStr = R"GLSL(
#version 450 core

// Attributes
layout(location = 0) in vec3 aPosition;

layout(location = 4) in vec4 aBoneIndices;
layout(location = 5) in vec4 aBoneWeights;

// Uniforms

uniform mat4 uModel;
uniform mat4 uSkinningMatrices[128];

void main()
{
    vec3 totalPosition = vec3(1.0f);

    for(int i = 0 ; i < 4 ; i++)
    {
        if(aBoneIndices[i] == -1) continue;
        if(aBoneIndices[i] >= 128) 
        {
            totalPosition = aPosition;
            break;
        }
        vec4 localPosition = uSkinningMatrices[int(aBoneIndices[i])] * vec4(aPosition, 1.0f);
        totalPosition += (localPosition * aBoneWeights[i]).rgb;
    }

    gl_Position = uModel * vec4(totalPosition,1.0);
})GLSL";

static const char* cascadeShadowMapGeometryShaderStr = R"GLSL(
#version 450 core

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;
    
uniform mat4 uDirectionalMatrix[5];
uniform int  uIndex;

int CascadeCount = 4;
    
void main()
{          
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = uDirectionalMatrix[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = uIndex * CascadeCount + gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
})GLSL";

static const char* cascadeShadowMapFragmentShaderStr = R"GLSL(
#version 450 core

layout(location = 0) out float aFragDepth;

void main()
{
})GLSL";
#pragma endregion

ShadowProcess::ShadowProcess()
{
    //  Default values (will be overidden by camera value, this is done to avoid warning)
    m_cascadeDistances[0] = 500.f / 10.0f;
    m_cascadeDistances[1] = 500.f / 7.50f;
    m_cascadeDistances[2] = 500.f / 2.00f;
    m_cascadeDistances[3] = 500.f;
}


ShadowProcess::~ShadowProcess()
{

}
void ShadowProcess::Initialize()
{
    EngineContext& engineContext = EngineContext::Instance();

    m_meshShadowmapShader.Generate(meshShadowMapVertexShaderStr, shadowMapFragmentShaderStr);
    m_skeletalMeshShadowmapShader.Generate(skeletalMeshShadowMapVertexShaderStr, shadowMapFragmentShaderStr);

    m_meshShadowCubemapShader.Generate(meshShadowCubeMapVertexShaderStr, shadowCubeMapFragmentShaderStr, shadowCubeMapGeometryShaderStr);
    m_skeletalMeshShadowCubemapShader.Generate(skeletalMeshShadowCubeMapVertexShaderStr, shadowCubeMapFragmentShaderStr, shadowCubeMapGeometryShaderStr);

    m_meshCascadeShadowmapShader.Generate(meshCascadeShadowMapVertexShaderStr, cascadeShadowMapFragmentShaderStr, cascadeShadowMapGeometryShaderStr);;
    m_skeletalMeshCascadeShadowmapShader.Generate(skeletalMeshCascadeShadowMapVertexShaderStr, cascadeShadowMapFragmentShaderStr, cascadeShadowMapGeometryShaderStr);;

    //  DEPTH MAP FrameBuffer   
    {
        GLTextureArray& depthMap = SystemManager::GetRenderSystem().GPUWrapper.GeneratedDepthMapTextureArray;
        glGenFramebuffers(1, &m_depthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);

        glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap.GetID());
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap.GetID(), 0, 0);
   
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Logger::Error("ShadowProcess - Initialize - DepthMap Framebuffer is not complete !");
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    //  DEPTH CUBE MAP FrameBuffer
    {
        GLTextureArray& depthCubeMap = SystemManager::GetRenderSystem().GPUWrapper.GeneratedDepthCubeMapTextureArray;
        glGenFramebuffers(1, &m_depthCubeMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthCubeMapFBO);

        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, depthCubeMap.GetID());
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap.GetID(), 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Logger::Error("ShadowProcess - Initialize - DepthCubeMap Framebuffer is not complete !");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    //  CASCADE DEPTH MAP FrameBuffer   
    {
        GLTextureArray& cascadeDepthMap = SystemManager::GetRenderSystem().GPUWrapper.GeneratedCascadeDepthMapTextureArray;
        glGenFramebuffers(1, &m_cascadeDepthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_cascadeDepthMapFBO);

        glBindTexture(GL_TEXTURE_2D_ARRAY, cascadeDepthMap.GetID());
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cascadeDepthMap.GetID(), 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Logger::Error("ShadowProcess - Initialize - Cascade DepthMap Framebuffer is not complete !");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    glCreateBuffers(1, &m_ShadowMapUBO);
    size_t s = sizeof(Matrix4) * Max_Lights_Count + sizeof(Matrix4) * Cascade_Count * Max_Lights_Count + sizeof(float) * 4 * Cascade_Count;
    glNamedBufferData(m_ShadowMapUBO, s, NULL, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, UBOIndex_Shadows, m_ShadowMapUBO, 0, s);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void ShadowProcess::ProcessShadowMaps()
{
    Camera* cam = SystemManager::GetCameraSystem().renderingCamera;

    //  Sub frustum fars values (Maximum cascade far value must be 500.f)
    float maxCascadeFar = Maths::Min(cam->GetFar(), 500.f);
    m_cascadeDistances[0] = maxCascadeFar / 75.f;
    m_cascadeDistances[1] = maxCascadeFar / 40.f;
    m_cascadeDistances[2] = maxCascadeFar / 20.f;
    m_cascadeDistances[3] = maxCascadeFar / 10.f;

    //  Process pre render phase

    int savedFB = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &savedFB);
    glViewport(0, 0, 1024, 1024);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    //  Make polygon that are not in view renderer to avoid artifacts
    glEnable(GL_DEPTH_CLAMP);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.f, 1.f);

    //  Clear only once depthCubeMap frame buffer since we render all textures in one texture Cube array
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthCubeMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, m_cascadeDepthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);


    EngineContext& engineContext = EngineContext::Instance();

    //  Render phase
    //  Generate depth maps
    unsigned int count = 0;
    for (const auto& light : SystemManager::GetRenderSystem().GetAllLights())
    {
        if (light->castShadows)
        {
            switch (light->type)
            {
            case LightType::Spot:
                GetSpotLightViewProj(m_viewProjSpotLight[count], *light);
                GenerateSpotLightDepthMap(m_viewProjSpotLight[count], count);
                break;
            case LightType::Point:
                GetPointLightViewProj(m_viewProjPointLight[count], *light);
                GeneratePointLightDepthCubeMap(m_viewProjPointLight[count], count);
                break;
            case LightType::Directional:
                GetDirectionalLightViewProj(m_viewProjDirectional[count], *light);
                GenerateDirectionalLightCascadeDepthMap(m_viewProjDirectional[count], count);
                break;
            default: break;
            }
        }
       
        if (count++; count > Max_Lights_Count) break;
    }

    //  Post render phase

    glDisable(GL_POLYGON_OFFSET_FILL);

    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, savedFB);
    glDisable(GL_DEPTH_CLAMP);
    glEnable(GL_CULL_FACE);

    //  Fille shadows uniform buffers
    size_t offset = 0;
    size_t size = sizeof(Matrix4) * Max_Lights_Count;
    glNamedBufferSubData(m_ShadowMapUBO, offset, size, m_viewProjSpotLight);

    offset += size;
    size = sizeof(Matrix4) * Cascade_Count * Max_Lights_Count;
    glNamedBufferSubData(m_ShadowMapUBO, offset, size, m_viewProjDirectional);

    offset += size;
    size = sizeof(float) * Cascade_Count;
    glNamedBufferSubData(m_ShadowMapUBO, offset, size, m_cascadeDistances);
}



//  DEPTH MAP GENERATION FUNCTIONS
//  ------------------------------



void ShadowProcess::GenerateSpotLightDepthMap(const Matrix4& lightViewProj, int index)
{
    EngineContext& engineContext = EngineContext::Instance();
    GLTextureArray& depthMapTexture = SystemManager::GetRenderSystem().GPUWrapper.GeneratedDepthMapTextureArray;

    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);

    //  Attach correct texture to the frame buffer
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMapTexture.GetID());
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMapTexture.GetID(), 0, index);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Logger::Error("ShadowProcess - GenerateDepthMap - DepthMap Framebuffer is not complete!");
    }

    //  Since we use one texture each time we need to clear the buffer each time we render in one texture
    glClear(GL_DEPTH_BUFFER_BIT);

    //  Render scene from spot light view then past it in the binded texture
    RenderSpotLightScene(lightViewProj);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}


void ShadowProcess::GeneratePointLightDepthCubeMap(const Matrix4 lightViewProj[6], int index)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthCubeMapFBO);

    //  Render scene from point light view then past it in the binded cube map texture
    RenderPointLightScene(lightViewProj, index);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowProcess::GenerateDirectionalLightCascadeDepthMap(const Matrix4 lightViewProj[Cascade_Count], int index)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_cascadeDepthMapFBO);

    //  Render scene from the directional light orthographic view then past it in the binded cascade maps texture
    RenderDirectionalLightScene(lightViewProj, index);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//  DEPTH MAPS RENDER FUNCTIONS
//  ---------------------------


void ShadowProcess::RenderSpotLightScene(const Matrix4& lightViewProj)
{
    EngineContext& engineContext = EngineContext::Instance();
    RenderSystem& renderSystem = SystemManager::GetRenderSystem();

    //  MESH RENDERING

    renderSystem.GPUWrapper.meshVAO.Bind();
    m_meshShadowmapShader.Bind();

    for (const MeshInstance* instance : renderSystem.GetAllMeshInstances())
    {
        Matrix4 mvp = lightViewProj * instance->transform->GetWorldMatrix();
        m_meshShadowmapShader.SendUniform("uSpotLightMatrix", mvp.elements);

        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            static_cast<GLsizei>(instance->mesh->indices.size()),
            GL_UNSIGNED_INT,
            instance->mesh->indices.data(),
            instance->mesh->GPUData->offset
        );
    }
    m_meshShadowmapShader.Unbind();
    renderSystem.GPUWrapper.meshVAO.Unbind();

    //  SKELETAL MESH RENDERING

    renderSystem.GPUWrapper.skeletalMeshVAO.Bind();
    m_skeletalMeshShadowmapShader.Bind();

    for (const SkeletalMeshInstance* instance : renderSystem.GetAllSkeletalMeshInstances())
    {
        Matrix4 mvp = lightViewProj * instance->transform->GetWorldMatrix();
        m_skeletalMeshShadowmapShader.SendUniform("uSpotLightMatrix", mvp.elements);

        const std::vector<Matrix4>& bonesTransform = instance->skeleton->boneTransforms;
        m_skeletalMeshShadowmapShader.SendUniform("uSkinningMatrices[0]", bonesTransform.data(), static_cast<int>(bonesTransform.size()));

        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            static_cast<GLsizei>(instance->mesh->indices.size()),
            GL_UNSIGNED_INT,
            instance->mesh->indices.data(),
            instance->mesh->GPUData->offset
        );
    }
    m_skeletalMeshShadowmapShader.Unbind();
    renderSystem.GPUWrapper.skeletalMeshVAO.Unbind();
}



void ShadowProcess::RenderPointLightScene(const Matrix4 lightViewProj[6], int index)
{
    EngineContext& engineContext = EngineContext::Instance();
    RenderSystem& renderSystem = SystemManager::GetRenderSystem();

    Vector3 lightPosition = SystemManager::GetRenderSystem().GetAllLights()[index]->gameObject.transform.Position();

    //  MESH RENDERING

    renderSystem.GPUWrapper.meshVAO.Bind();
    m_meshShadowCubemapShader.Bind();
    m_meshShadowCubemapShader.SendUniform("uPointLightMatrix[0]", lightViewProj, 6);
    m_meshShadowCubemapShader.SendUniform("uIndex", &index);
    m_meshShadowCubemapShader.SendUniform("uPosition", &lightPosition);

    for (const MeshInstance* instance : renderSystem.GetAllMeshInstances())
    {
        m_meshShadowCubemapShader.SendUniform("uModel", instance->transform->GetWorldMatrix().elements);

        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            static_cast<GLsizei>(instance->mesh->indices.size()),
            GL_UNSIGNED_INT,
            instance->mesh->indices.data(),
            instance->mesh->GPUData->offset
        );
    }
    m_meshShadowCubemapShader.Unbind();
    renderSystem.GPUWrapper.meshVAO.Unbind();

    //  SKELETAL MESH RENDERING

    renderSystem.GPUWrapper.skeletalMeshVAO.Bind();
    m_skeletalMeshShadowCubemapShader.Bind();
    m_skeletalMeshShadowCubemapShader.SendUniform("uPointLightMatrix[0]", lightViewProj, 6);
    m_skeletalMeshShadowCubemapShader.SendUniform("uIndex", &index);
    m_skeletalMeshShadowCubemapShader.SendUniform("uPosition", &lightPosition);

    for (const SkeletalMeshInstance* instance : renderSystem.GetAllSkeletalMeshInstances())
    {
        const std::vector<Matrix4>& bonesTransform = instance->skeleton->boneTransforms;
        m_skeletalMeshShadowCubemapShader.SendUniform("uSkinningMatrices[0]", bonesTransform.data(), static_cast<int>(bonesTransform.size()));
        m_skeletalMeshShadowCubemapShader.SendUniform("uModel", instance->transform->GetWorldMatrix().elements);

        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            static_cast<GLsizei>(instance->mesh->indices.size()),
            GL_UNSIGNED_INT,
            instance->mesh->indices.data(),
            instance->mesh->GPUData->offset
        );
    }
    m_skeletalMeshShadowCubemapShader.Unbind();
    renderSystem.GPUWrapper.skeletalMeshVAO.Unbind();
}


void ShadowProcess::RenderDirectionalLightScene(const Matrix4 lightViewProj[Cascade_Count], int index)
{
    EngineContext& engineContext = EngineContext::Instance();
    RenderSystem& renderSystem = SystemManager::GetRenderSystem();

    Vector3 lightPosition = SystemManager::GetRenderSystem().GetAllLights()[index]->gameObject.transform.Position();

    //  MESH RENDERING

    renderSystem.GPUWrapper.meshVAO.Bind();
    m_meshCascadeShadowmapShader.Bind();
    m_meshCascadeShadowmapShader.SendUniform("uDirectionalMatrix[0]", lightViewProj, Cascade_Count);
    m_meshCascadeShadowmapShader.SendUniform("uIndex", &index);

    for (const MeshInstance* instance : renderSystem.GetAllMeshInstances())
    {
        if (instance->isActive == false)
            continue;

        m_meshCascadeShadowmapShader.SendUniform("uModel", instance->transform->GetWorldMatrix().elements);

        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            static_cast<GLsizei>(instance->mesh->indices.size()),
            GL_UNSIGNED_INT,
            instance->mesh->indices.data(),
            instance->mesh->GPUData->offset
        );
    }
    m_meshCascadeShadowmapShader.Unbind();
    renderSystem.GPUWrapper.meshVAO.Unbind();

    //  SKELETAL MESH RENDERING

    renderSystem.GPUWrapper.skeletalMeshVAO.Bind();
    m_skeletalMeshCascadeShadowmapShader.Bind();
    m_skeletalMeshCascadeShadowmapShader.SendUniform("uDirectionalMatrix[0]", lightViewProj, Cascade_Count);
    m_skeletalMeshCascadeShadowmapShader.SendUniform("uIndex", &index);

    for (const SkeletalMeshInstance* instance : renderSystem.GetAllSkeletalMeshInstances())
    {
        if (instance->isActive == false)
            continue;

        const std::vector<Matrix4>& bonesTransform = instance->skeleton->boneTransforms;
        m_skeletalMeshCascadeShadowmapShader.SendUniform("uSkinningMatrices[0]", bonesTransform.data(), static_cast<int>(bonesTransform.size()));
        m_skeletalMeshCascadeShadowmapShader.SendUniform("uModel", instance->transform->GetWorldMatrix().elements);

        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            static_cast<GLsizei>(instance->mesh->indices.size()),
            GL_UNSIGNED_INT,
            instance->mesh->indices.data(),
            instance->mesh->GPUData->offset
        );
    }
    m_skeletalMeshCascadeShadowmapShader.Unbind();
    renderSystem.GPUWrapper.skeletalMeshVAO.Unbind();
}


//  UTILS FUNCTIONS 
//  ---------------

std::array<Vector4,8> GetFrustumCornersWorldSpace(const Matrix4& proj, const Matrix4& view)
{
    //  Get frustum cornet by generating simples cubes corners, then multiplying them with transposed(for convention) inverse of our view proj matrices
    const Matrix4 inv = Matrix4::Transpose(Matrix4::Inverse(proj * view));

    std::array<Vector4, 8> frustumCorners;

    size_t i = 0;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                //  MV matrix * Local cube corner
                const Vector4 point = inv * Vector4(
                    2.0f * x - 1.0f,
                    2.0f * y - 1.0f,
                    2.0f * z - 1.0f,
                    1.0f
                );

                frustumCorners[i] = point / point.w;
                i++;
            }
        }
    }

    return frustumCorners;
}

struct CameraDirection
{
    GLenum CubemapFace;
    Vector3 Target;
    Vector3 Up;
};

CameraDirection CameraDirections[6] =
{
    { GL_TEXTURE_CUBE_MAP_POSITIVE_X, Vector3::Right,   Vector3::Down },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, Vector3::Left,    Vector3::Down },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, Vector3::Up,      Vector3::Forward },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, Vector3::Down,    Vector3::Back },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, Vector3::Forward, Vector3::Down },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, Vector3::Back,    Vector3::Down }
};

//  VIEW PROJ COMPUTE FUNCTIONS
//  ---------------------------

void ShadowProcess::GetSpotLightViewProj(Matrix4& lightViewProj, const LightComponent& light)
{
    /*
    The spot light view proj matrix calcultion is the simplest one, we compute a projection matrix by using a persepctive matrix function, the view matrix take the position and the direction of the spot light.
    */

    Vector3 pos = light.gameObject.transform.Position();
    Vector3 dir = light.gameObject.transform.Forward();
    Vector3 target = pos + dir;

    //  Simply compute spot light perspective projection and view matrices

    Matrix4 depthProj = Matrix4::Perspective(Maths::Acos(light.outerCutoff) * 2.f, 1.0f, 2.f, 50.0f);
    Matrix4 depthView = Matrix4::LookAt(pos, target, Vector3::Up);

    lightViewProj = depthProj * depthView;
}


void ShadowProcess::GetPointLightViewProj(Matrix4 lightViewProj[6], const LightComponent& light)
{
    /*
    The point light emit in every direction so it needs to render in a cube around itself, this why the point light count 6 viewproj matrices.
    The view projection matrices calculation is almost the same as the spot light view proj matrix but  we produce it for each face.
    */
    Vector3 position = light.gameObject.transform.Position();

    //  Compute perspective projection and view matrices for each face around the point light
    for (int i = 0; i < 6; i++)
    {
        Vector3 target = position + CameraDirections[i].Target;
    
        Matrix4 depthProj = Matrix4::Perspective(Maths::PIHalf, 1.0f, 0.05f, 25.f);
        Matrix4 depthView = Matrix4::LookAt(position, target, CameraDirections[i].Up);
    
        lightViewProj[i] = depthProj * depthView;
    }
}

void ShadowProcess::GetDirectionalLightViewProj(Matrix4 lightViewProj[Cascade_Count], const LightComponent& light)
{
    /*
    Here is the most complex method among the 3 light types shadow viewproj matrix calulation. We want to compute the view-proj matrix of the directional light so it targets the camera view target, and we want the shadows to be display when far enough from the camera.
    The solution is to divide the camera frustum in multiple sub-frustum, so the number of light view projection matrices will be the same as the cascade count(number of sub-frustum). 
    Each N view matrix of the light will look at the center of the N th sub-frustum, the N th orthographic matrix will be computed with the N th sub-frustum AABB
    */

    Vector3 dir = light.gameObject.transform.Forward();
    Camera* cam = SystemManager::GetCameraSystem().renderingCamera;

    float near = cam->GetNear();
    float radFOV = cam->GetFOV() * Maths::DEGTORAD;
    float aspect = cam->GetAspect();

    for (int i = 0; i < Cascade_Count; i++)
    {
        float far = m_cascadeDistances[i];

        //  Get current sub-frustum
        Matrix4 cascadeProj = Matrix4::Perspective(radFOV, aspect, near, far);
        std::array<Vector4, 8> wCorners = GetFrustumCornersWorldSpace(cascadeProj, cam->GetViewMatrix());

        //  Get center of the current sub-frustum
        Vector3 frustumCenter = Vector3::Zero;
        for (const Vector4& wCornerCoord : wCorners)
        {
            frustumCenter += wCornerCoord.xyz;
        }
        frustumCenter /= static_cast<float>(wCorners.size());

        //  Create view matrix looking at the center of the frustum from the directional light position
        Matrix4 depthView = Matrix4::LookAt(frustumCenter, frustumCenter + dir, Vector3::Up);

        //  Generate sub-frustum AABB

        float minX, minY, minZ, maxX, maxY, maxZ;
        minX = minY = minZ = std::numeric_limits<float>::max();
        maxX = maxY = maxZ = std::numeric_limits<float>::min();

        for (const Vector4& coord : wCorners)
        {
            const auto trf = Matrix4::Transpose(depthView) * coord;
            minX = Maths::Min(minX, trf.x);
            minY = Maths::Min(minY, trf.y);
            minZ = Maths::Min(minZ, trf.z);

            maxX = Maths::Max(maxX, trf.x);
            maxY = Maths::Max(maxY, trf.y);
            maxZ = Maths::Max(maxZ, trf.z);
        }

        //  Set the orthogonal matrix of the directional light so it fits the sub-frustum AABB
        Matrix4 depthProj = Matrix4::Orthographic(minX, maxX, minY, maxY, -1.f * maxZ, -1.f * minZ);
        lightViewProj[i] = depthProj * depthView;

        near = far;
    }
}
#include "Renderer/MeshUnlitShader.hpp"

#include <glad/gl.h>

#include "ECS/MeshComponent.hpp"
#include "ECS/SkeletalMeshComponent.hpp"
#include "ECS/Transform.hpp"
#include "Renderer/GPUMeshData.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Renderer/MeshInstance.hpp"
#include "Resources/Resource/Texture.hpp"
#include "EngineContext.hpp"

#pragma region UNLIT SHADER REGION

static const char* unlitVertexShaderStr = R"GLSL(
#version 450 core

// Attributes

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;

//  Uniform Buffers

layout (std140, binding = 0) uniform UBO_matrices
{
    mat4 uProjection;
    mat4 uView;
    vec3 uViewPos;
};

// Uniforms

uniform mat4 uModel;

//  PACKED MAT :
//        Xoffset         |      Yoffset     |      Utiling      |      Vtiling      
//       Rdiffuse         |     Gdiffuse     |     Bdiffuse      |     Adiffuse      
//       Remissive        |     Gemissive    |     Bemissive     |        ...        
//    emissiveStrength    |    useDiffuse    |    useEmissive    |      useMask      

uniform mat4 uPackedMat;

// Varyings

out VS_OUT
{
    vec2 uv;
}vs_out;


//  Functions

void main()
{
    vec2 offset = uPackedMat[0].xy;
    vec2 tiling = uPackedMat[0].zw;
    vs_out.uv = offset + aUV * tiling;

    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
})GLSL";


static const char* skeletalUnlitVertexShaderStr = R"GLSL(
#version 450 core

// Attributes

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;
//  2, 3 ... unused locations
layout(location = 4) in vec4 aBoneIndices;
layout(location = 5) in vec4 aBoneWeights;

//  Constants

const int boneMaxCount = 128;

//  Uniform Buffers

layout (std140, binding = 0) uniform UBO_matrices
{
    mat4 uProjection;
    mat4 uView;
    vec3 uViewPos;
};

// Uniforms

uniform mat4 uModel;

//  PACKED MAT :
//        Xoffset         |      Yoffset     |      Utiling      |      Vtiling      
//       Rdiffuse         |     Gdiffuse     |     Bdiffuse      |     Adiffuse      
//       Remissive        |     Gemissive    |     Bemissive     |        ...        
//    emissiveStrength    |    useDiffuse    |    useEmissive    |      useMask      

uniform mat4 uPackedMat;

uniform mat4 uSkinningMatrices[boneMaxCount];

// Varyings

out VS_OUT
{
    vec2 uv;
}vs_out;


//  Functions

void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < 4 ; i++)
    {
        if(aBoneIndices[i] == -1) 
            continue;
        if(aBoneIndices[i] >= boneMaxCount) 
        {
            totalPosition = vec4(aPosition, 1.0f);
            break;
        }
        
        vec4 localPosition = uSkinningMatrices[int(aBoneIndices[i])] * vec4(aPosition, 1.0f);
        totalPosition += localPosition * aBoneWeights[i];
    }

    vec2 offset = uPackedMat[0].xy;
    vec2 tiling = uPackedMat[0].zw;
    vs_out.uv = offset + aUV * tiling;

    gl_Position = uProjection * uView * uModel * vec4(totalPosition.xyz, 1.0);
})GLSL";

static const char* unlitFragmentShaderStr = R"GLSL(
#version 450 core

//  Outputs

layout (location = 0) out vec4 oColor;

// Varyings

in VS_OUT
{
    vec2 uv;
} fs_in;

//  Uniforms

uniform sampler2D uDiffuseTexture;
uniform sampler2D uEmissiveTexture;
uniform sampler2D uMaskTexture;

//  PACKED MAT :
//        Xoffset         |      Yoffset     |      Utiling      |      Vtiling      
//       Rdiffuse         |     Gdiffuse     |     Bdiffuse      |     Adiffuse      
//       Remissive        |     Gemissive    |     Bemissive     |        ...        
//    emissiveStrength    |    useDiffuse    |    useEmissive    |      useMask      

uniform mat4 uPackedMat;

//  Functions

vec4 GetTexturePixel(in sampler2D text, in vec2 uv, in float isUsed, in float isDefaultWhite)
{
    return isDefaultWhite * (1.0 - isUsed) + isUsed * texture(text, uv);
}

void main()
{
    bool useMaskTexture = uPackedMat[3][3] == 1.0;
    if(useMaskTexture)
    {
        if(texture(uMaskTexture, fs_in.uv).r < 0.1f) discard;
    }
    
    vec4 DiffuseColor  = uPackedMat[1];
    vec3 EmissiveColor = uPackedMat[2].rgb;
    float emissiveStrength   = uPackedMat[3][0];
    float useDiffuseTexture  = uPackedMat[3][1];
    float useEmissiveTexture = uPackedMat[3][2];
    
    vec3 toLinear = vec3(2.2);

    vec3 emissiveTexture = pow(GetTexturePixel(uEmissiveTexture, fs_in.uv,useEmissiveTexture, 1.0).rgb, toLinear);
    EmissiveColor *= emissiveTexture * emissiveStrength;

    vec4 diffuseTexture = GetTexturePixel(uDiffuseTexture, fs_in.uv,useDiffuseTexture, 1.0);
    diffuseTexture.rgb  = pow(diffuseTexture.rgb, toLinear);
    DiffuseColor *= diffuseTexture; 
    
    oColor =  DiffuseColor + vec4(EmissiveColor,0.0);
})GLSL";

#pragma endregion


void MeshUnlitShader::Initialize()
{
    //  Load Shader

    m_meshShader.Generate(unlitVertexShaderStr, unlitFragmentShaderStr);
    m_skeletalMeshShader.Generate(skeletalUnlitVertexShaderStr, unlitFragmentShaderStr);

    glUseProgram(m_meshShader.GetID());
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uDiffuseTexture"), 0);
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uEmissiveTexture"), 1);
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uMaskTexture"), 2);

    m_meshLocations.packedMat1 = m_meshShader.GetLocationFromUniformName("uPackedMat");
    m_meshLocations.matModel = m_meshShader.GetLocationFromUniformName("uModel");

    glUseProgram(m_skeletalMeshShader.GetID());
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uDiffuseTexture"), 0);
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uEmissiveTexture"), 1);
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uMaskTexture"), 2);

    m_skMeshLocations.packedMat1    = m_skeletalMeshShader.GetLocationFromUniformName("uPackedMat");
    m_skMeshLocations.matModel      = m_skeletalMeshShader.GetLocationFromUniformName("uModel");
    m_skMeshLocations.matSkin       = m_skeletalMeshShader.GetLocationFromUniformName("uSkinningMatrices[0]");
}

struct unlitShaderPackedMat4
{
    alignas(8) Vector2 offset;
    alignas(8) Vector2 tiling;

    alignas(16) Color4  diffuseColor;
    alignas(16) Color3  emissiveColor;

    alignas(4) float emissiveStrength;
    alignas(4) float  useDiffusetexture;
    alignas(4) float  useEmissivetexture;
    alignas(4) float  useMaskTexture;
};

void MeshUnlitShader::SendMaterialDatas(MaterialData& material, PBRUniformsLocation const& locations) const
{
    unlitShaderPackedMat4 packedMat =
    {
        material.offset,
        material.tiling,
        material.diffuseColor,
        material.emissiveColor,
        material.emissiveStrength,
        static_cast<float>(material.diffuseTexture != nullptr),
        static_cast<float>(material.emissiveTexture != nullptr),
        static_cast<float>(material.maskTexture != nullptr)
    };

    glUniformMatrix4fv(locations.packedMat1, 1, GL_FALSE, &packedMat.offset.x);

    if (packedMat.useDiffusetexture)  material.diffuseTexture->GPUData->generatedTexture->BindUnit(0);
    if (packedMat.useEmissivetexture) material.emissiveTexture->GPUData->generatedTexture->BindUnit(1);
    if (packedMat.useMaskTexture)     material.maskTexture->GPUData->generatedTexture->BindUnit(2);
}


void MeshUnlitShader::DrawMesh(const Matrix4& model, const MeshData& mesh, const MaterialData* material) const
{
    glUniformMatrix4fv(m_meshLocations.matModel, 1, GL_FALSE, model.elements);

    const MaterialData* mat = material ? material : m_defaultMat;
    if (mat)
    {
        SendMaterialDatas(const_cast<MaterialData&>(*mat), m_meshLocations);
    }

    glDrawElementsBaseVertex(
        GL_TRIANGLES,
        static_cast<GLsizei>(mesh.indices.size()),
        GL_UNSIGNED_INT,
        mesh.indices.data(),
        mesh.GPUData->offset
    );

    glBindTextureUnit(0, 0);
    glBindTextureUnit(1, 0);
}


void MeshUnlitShader::DrawSkeletalMesh(const Matrix4& model, const MeshData& mesh, const SkeletalData& skeleton, const MaterialData* material) const
{
    glUniformMatrix4fv(m_skMeshLocations.matModel, 1, GL_FALSE, model.elements);
    glUniformMatrix4fv(m_skMeshLocations.matSkin, static_cast<GLsizei>(skeleton.boneTransforms.size()), GL_FALSE, skeleton.boneTransforms[0].elements);

    const MaterialData* mat = material ? material : m_defaultMat;
    if (mat)
    {
        SendMaterialDatas(const_cast<MaterialData&>(*mat), m_skMeshLocations);
    }

    glDrawElementsBaseVertex(
        GL_TRIANGLES,
        static_cast<GLsizei>(mesh.indices.size()),
        GL_UNSIGNED_INT,
        mesh.indices.data(),
        mesh.GPUData->offset
    );

    glBindTextureUnit(0, 0);
    glBindTextureUnit(1, 0);
}
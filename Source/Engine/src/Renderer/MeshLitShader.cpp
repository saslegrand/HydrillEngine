#include "Renderer/MeshLitShader.hpp"

#include <glad/gl.h>

#include "ECS/MeshComponent.hpp"
#include "ECS/SkeletalMeshComponent.hpp"
#include "ECS/Transform.hpp"
#include "Renderer/GPUMeshData.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Renderer/MeshInstance.hpp"
#include "Resources/Resource/Material.hpp"
#include "Resources/Resource/Texture.hpp"
#include "Resources/Resource/Texture.hpp"
#include "EngineContext.hpp"

#pragma region LIT SHADER REGION

static const char* litVertexShaderStr = R"GLSL(
#version 450 core

// Attributes

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;

//  Uniform Buffers

layout (std140, binding = 0) uniform UBO_camera
{
    mat4 uProjection;
    mat4 uView;
    vec3 uViewPos;
};

// Uniforms

uniform mat4 uModel;
uniform mat4 uModelNormalMatrix;

//  PACKED MAT :
//        Xoffset         |      Yoffset     |      Utiling      |      Vtiling      
//    emissiveStrength    |    useDiffuse    |    useSpecular    |    useEmissive    
//       useMask          |       ...        |        ...        |        ...        
//         ...            |       ...        |        ...        |        ...        

uniform mat4 uPackedMat;

// Varyings

out VS_OUT
{
    vec2 uv;
    vec3 pos;
    vec3 normal;
    mat3 TBN;
}vs_out;


//  Functions

mat3 ComputeTBN()
{
    vec3 T = normalize(mat3(uModel) * aTangent);
    vec3 N =  normalize(mat3(uModel)* aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    return mat3(T, B, N); 
}

void main()
{
    vec4 pos4 = uModel * vec4(aPosition, 1.0);

    vec2 offset = uPackedMat[0].xy;
    vec2 tiling = uPackedMat[0].zw;
    vs_out.uv = offset + aUV * tiling;

    vs_out.pos    = pos4.xyz / pos4.w;
    vs_out.normal = normalize(vec3(uModelNormalMatrix * vec4(aNormal, 0.0)));
    vs_out.TBN    = ComputeTBN();

    gl_Position   = uProjection * uView * pos4;
})GLSL";


static const char* skeletalLitVertexShaderStr = R"GLSL(
#version 450 core

// Attributes

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec4 aBoneIndices;
layout(location = 5) in vec4 aBoneWeights;

//  Constants

const int boneMaxCount = 128;

//  Uniform Buffers

layout (std140, binding = 0) uniform UBO_camera
{
    mat4 uProjection;
    mat4 uView;
    vec3 uViewPos;
};


// Uniforms

uniform mat4 uModel;
uniform mat4 uModelNormalMatrix;

//  PACKED MAT :
//        Xoffset         |      Yoffset     |      Utiling      |      Vtiling      
//    emissiveStrength    |    useDiffuse    |    useSpecular    |    useEmissive    
//       useMask          |       ...        |        ...        |        ...        
//         ...            |       ...        |        ...        |        ...        

uniform mat4 uPackedMat;


uniform mat4 uSkinningMatrices[boneMaxCount];

// Varyings

out VS_OUT
{
    vec2 uv;
    vec3 pos;
    vec3 normal;
    mat3 TBN;
}vs_out;


//  Functions

mat3 ComputeTBN(vec3 normal)
{
    vec3 T = normalize(mat3(uModel) * aTangent);
    vec3 N =  normalize(mat3(uModel)* normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    return mat3(T, B, N); 
}

void main()
{
    vec4 totalPosition = vec4(0.0f);
    vec3 localNormal = aNormal;

    for(int i = 0 ; i < 4 ; i++)
    {
        if(aBoneIndices[i] == -1) 
            continue;
        if(aBoneIndices[i] >= boneMaxCount) 
        {
            totalPosition = vec4(aPosition, 1.0f);
            break;
        }
        
        if(i==0) localNormal = mat3(uSkinningMatrices[int(aBoneIndices[0])]) * localNormal;
        vec4 localPosition = uSkinningMatrices[int(aBoneIndices[i])] * vec4(aPosition, 1.0f);
        totalPosition += localPosition * aBoneWeights[i];
    }

    vec4 pos4 = uModel * vec4(totalPosition.xyz, 1.0);

    vec2 offset = uPackedMat[0].xy;
    vec2 tiling = uPackedMat[0].zw;
    vs_out.uv = offset + aUV * tiling;

    vs_out.pos    = pos4.xyz / pos4.w;
    vs_out.normal = normalize(vec3(uModelNormalMatrix * vec4(localNormal, 0.0)));
    vs_out.TBN    = ComputeTBN(localNormal);

    gl_Position   = uProjection * uView * pos4;
})GLSL";

static const char* litFragmentShaderStr = R"GLSL(
#version 450 core

//  Outputs

layout (location = 0) out vec4 oColor;

// Varyings

in VS_OUT
{
    vec2 uv;
    vec3 pos;
    vec3 normal;
    mat3 TBN;
} fs_in;

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


struct shadeLightResult
{
	vec3 ambient; 
	vec3 diffuse;
	vec3 specular;
};

//  Uniforms

layout (std140, binding = 0) uniform UBO_camera
{
    mat4 uProjection;
    mat4 uView;
    vec3 uViewPos;
};

layout (std140, binding = 1) uniform UBO_Lights
{
    Light uLights[8];
    int   uNumLights;
};

layout (binding = 10) uniform sampler2DArrayShadow uDepthMap;
layout (binding = 11) uniform samplerCubeArrayShadow uDepthCubeMap;
layout (binding = 12) uniform sampler2DArrayShadow uCascadeDepthMap;

layout (std140, binding = 2) uniform UBO_ShadowMaps
{
    mat4  uSpotLightMatrix[8];
    mat4  uDirectionalLightMatrix[32];
    vec4  uDirectionalLightPlaneDistances;
};

layout (std140, binding = 3) uniform UBO_Settings
{
    bool uShadowsEnabled;
};

uniform sampler2D uDiffuseTexture;
uniform sampler2D uNormalTexture;
uniform sampler2D uSpecularTexture;
uniform sampler2D uEmissiveTexture;
uniform sampler2D uMaskTexture;

//  PACKED MAT COLORS :
//       Rdiffuse         |     Gdiffuse     |     Bdiffuse      |     Adiffuse      
//       Rambient         |     Gambient     |     Bambient      |       ...         
//       Rspecular        |     Gspecular    |     Bspecular     |       ...         
//       Remissive        |     Gemissive    |     Bemissive     |       ...         

uniform mat4  uMatColors;

//  PACKED MAT :
//        Xoffset         |      Yoffset     |      Utiling      |      Vtiling      
//    emissiveStrength    |    useDiffuse    |     useNormal     |    useSpecular    
//      useEmissive       |     useMask      |        ...        |        ...        
//         ...            |       ...        |        ...        |        ...        

uniform mat4 uPackedMat;

//  Global variables

int    cascadeCount = 4;
int    PCF = 3;
int    PCFSampleCount = (1 + 2 * PCF) * (1 + 2 * PCF);
float  PCFFactor = 1.0 / PCFSampleCount;
float  maxBias = 0.005;
float  minBias = maxBias * 0.01;

vec3 Normal;

//  Functions

//  Comparisons

float and(float a, float b) {
  return a * b;
}

float or(float a, float b) {
  return min(a + b, 1.0);
}

float not(float a) {
  return 1.0 - a;
}

float isEquals(float x, float y) {
  return 1.0 - abs(sign(x - y));
}

float isNotEqual(float x, float y) {
  return abs(sign(x - y));
}

float isGreater(float x, float y) {
  return max(sign(x - y), 0.0);
}

float isLower(float x, float y) {
  return max(sign(y - x), 0.0);
}

float isGreaterOrEqual(float x, float y) {
  return 1.0 - isLower(x, y);
}

float isLowerOrEqual(float x, float y) {
  return 1.0 - isGreater(x, y);
}


float spotLightIntensity(vec3 fragToLight, vec3 lightDirection, float innerCutoff, float outerCutoff)
{
    float theta = dot(fragToLight, normalize(-lightDirection)); 
	float epsilon = (innerCutoff - outerCutoff);
	return clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
}

float DirectionalLightShadowCompute(int idx, vec3 Direction)
{
    // select cascade layer
    vec4 fragPosViewSpace = uView * vec4(fs_in.pos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);
    
    int idxOffset = idx * cascadeCount;
    int layer = cascadeCount;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < uDirectionalLightPlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if(layer == cascadeCount) return 0.0;
    
    vec4 fragPosLightSpace = uDirectionalLightMatrix[layer + idxOffset] * vec4(fs_in.pos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z  > 1.0) return 0.0;

    // calculate bias (based on depth map resolution and slope)
    float bias = max(0.05 * (1.0 - dot(Normal, Direction)), 0.005);
    bias *= 1 / (uDirectionalLightPlaneDistances[layer] * 0.5);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uCascadeDepthMap, 0).xy;
   
    for(int x = -PCF; x <= PCF; ++x)
    {
        for(int y = -PCF; y <= PCF; ++y)
        {
            shadow += 1.0 - texture(uCascadeDepthMap, vec4(projCoords.xy + vec2(x, y) * texelSize, idxOffset + layer, projCoords.z - bias)).r; 
        }    
    }    
    return shadow * PCFFactor;
}

float spotLightShadowCompute(int idx, vec3 Direction)
{
    vec4 fragPosLightSpace = uSpotLightMatrix[idx] * vec4(fs_in.pos.xyz, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) return 0.0;

    //float closestDepth = texture(uDepthMap, vec3(projCoords.xy,idx)).r;

    float bias = max(0.005 * (1.0 - dot(Normal, normalize(-Direction))), 0.005);
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(uDepthMap, 0).xy;

    for(int x = -PCF; x <= PCF; ++x)
    {
        for(int y = -PCF; y <= PCF; ++y)
        {
            shadow += 1.0 - texture(uDepthMap, vec4(projCoords.xy + vec2(x, y) * texelSize,idx, projCoords.z - bias)).r;   
        }
    }
    return shadow * PCFFactor;
}

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float pointLightShadowCompute(int idx, vec3 LightPos, float FarPlane)
{
    vec3 lightToFrag = fs_in.pos - LightPos;
    //float closestDepth = texture(uDepthCubeMap, vec4(lightToFrag,idx),1.0).r * FarPlane;
    float currentDepth = length(lightToFrag);

    float shadow = 0.0;
    vec3 lightDir = LightPos - fs_in.pos;
    float lightDistance = length(lightDir);
    float viewDistance = length(uViewPos - fs_in.pos);
    float bias = 0.15 * lightDistance;

    float diskRadius = (1.0 + (viewDistance / FarPlane)) / 100.0;
    float samples = 20.0;

    for(int i = 0; i < samples; ++i)
    {
        shadow += 1.0 - texture(uDepthCubeMap, vec4(lightToFrag + gridSamplingDisk[i] * diskRadius, idx), (currentDepth - bias)/FarPlane).r;
    }

    return shadow / float(samples);
}

float computeVisibility(Light light, int current)
{
    if(!uShadowsEnabled || !light.castShadows) return 1.0;

    float visibility = 1.0;

    if(light.type == 2)
    {
        visibility = 1.0 - spotLightShadowCompute(current, light.direction);
    }
    else if (light.type == 1)
    {
        visibility = 1.0 - pointLightShadowCompute(current, light.position, light.radius);
    }
    else
    {
        visibility = 1.0 - DirectionalLightShadowCompute(current, light.direction);
    }

    return visibility;
}

shadeLightResult shadeLight(Light light, float shininess, vec3 position, vec3 normal)
{
	shadeLightResult r = shadeLightResult(vec3(0.0), vec3(0.0), vec3(0.0));

    vec3 lightDir = vec3(0.0);

    float isDirectional = isLowerOrEqual(light.type, 0);
    float isSpot = isGreaterOrEqual(light.type, 2);
    
    //	Point/Spot Light

    vec3 lightPosFromVertexPos = light.position - position;
    lightDir += not(isDirectional) * normalize(lightPosFromVertexPos);
    float dist = length(lightPosFromVertexPos);
    float attenuation = isDirectional + not(isDirectional) * clamp(1.0 - dist*dist/(light.radius*light.radius), 0.0, 1.0);
    attenuation*= attenuation;

    if (attenuation < 0.001) return r;

    // Directional light
    lightDir += isDirectional * -normalize(light.direction);

    vec3 eyeDir     = normalize(uViewPos - position);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specAngle = max(dot(eyeDir, reflectDir), 0.0);

    r.ambient  = light.ambient;
    float diffuseFactor = max(dot(normal, lightDir),0.0);
    r.diffuse  = attenuation * light.diffuse  * (light.intensity * diffuseFactor);
    r.specular = diffuseFactor * attenuation * light.specular * (pow(specAngle, shininess / 4.0));
	r.specular = clamp(r.specular, 0.0, 1.0);

	//	Spot Light

    float spotIntensity = not(isSpot) +  isSpot * spotLightIntensity(lightDir, light.direction, light.cutOff.x, light.cutOff.y);
	r.diffuse  *= spotIntensity;
	r.specular *= spotIntensity;

	return r;
}

shadeLightResult getLightsShading()
{
    shadeLightResult lightResult = shadeLightResult(vec3(0.0), vec3(0.0), vec3(0.0));

	for (int i = 0; i < uNumLights; ++i)
    {
        Light currlight =  uLights[i];

        if(!currlight.enabled) continue;

        float visibility = computeVisibility(uLights[i], i);

        shadeLightResult light = shadeLight(currlight, 32.f,fs_in.pos, normalize(Normal));
        lightResult.ambient  += light.ambient;
        lightResult.diffuse  += light.diffuse * visibility;
        lightResult.specular += light.specular * visibility;
    }
    return lightResult;
}

vec4 GetTexturePixel(in sampler2D text, in vec2 uv, in float isUsed, in float isDefaultWhite)
{
    return isDefaultWhite * (1.0 - isUsed) + isUsed * texture(text, uv);
}

void main()
{
    bool useMaskTexture = uPackedMat[2][1] == 1.0;
    if(useMaskTexture)
    {
        if(texture(uMaskTexture, fs_in.uv).r < 0.1f) discard;
    }

    float emissiveStrength    = uPackedMat[1][0];
    float useDiffuseTexture   = uPackedMat[1][1];
    float useNormalTexture    = uPackedMat[1][2];
    float useSpecularTexture  = uPackedMat[1][3];
    float useEmissiveTexture  = uPackedMat[2][0];
    
    Normal = (1.0 - useNormalTexture) * fs_in.normal + useNormalTexture * normalize(fs_in.TBN * normalize(texture(uNormalTexture, fs_in.uv).rgb * 2.0 - 1.0));

    vec4 diffuseColor  = uMatColors[0];
    vec3 ambientColor  = uMatColors[1].rgb;
    vec3 specularColor = uMatColors[2].rgb;
    vec3 emissiveColor = uMatColors[3].rgb;

    //  To linear space
    vec3 toLinear = vec3(2.2);

    vec4 diffuseTexture = GetTexturePixel(uDiffuseTexture, fs_in.uv,useDiffuseTexture, 1.0);
    diffuseTexture.rgb  = pow(diffuseTexture.rgb, toLinear);
    diffuseColor *= diffuseTexture;

    float specularTexture = GetTexturePixel(uSpecularTexture, fs_in.uv,useSpecularTexture, 1.0).r;
    specularColor *= specularTexture;

    vec3 emissiveTexture = pow(GetTexturePixel(uEmissiveTexture, fs_in.uv,useEmissiveTexture, 1.0).rgb, toLinear);
    emissiveColor *= emissiveTexture * emissiveStrength;

    // Compute phong shading
    shadeLightResult lightResult = getLightsShading();

    diffuseColor.rgb *= lightResult.diffuse;
    ambientColor  *= lightResult.ambient;
    specularColor *= clamp(lightResult.specular,0.0, 1.0);

    // Apply light color
    oColor = diffuseColor;
    oColor.rgb += ambientColor + specularColor + emissiveColor;
})GLSL";

#pragma endregion



void MeshLitShader::Initialize()
{
    //  Load Shader

    m_meshShader.Generate(litVertexShaderStr, litFragmentShaderStr);
    m_skeletalMeshShader.Generate(skeletalLitVertexShaderStr, litFragmentShaderStr);

    glUseProgram(m_meshShader.GetID());
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uDiffuseTexture"), 0);
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uNormalTexture"), 1);
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uSpecularTexture"), 2);
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uEmissiveTexture"), 3);
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uMaskTexture"), 4);

    m_meshLocations.matColor        = m_meshShader.GetLocationFromUniformName("uMatColors");
    m_meshLocations.packedMat1      = m_meshShader.GetLocationFromUniformName("uPackedMat");
    m_meshLocations.matModel        = m_meshShader.GetLocationFromUniformName("uModel");
    m_meshLocations.matModelNormal  = m_meshShader.GetLocationFromUniformName("uModelNormalMatrix");

    glUseProgram(m_skeletalMeshShader.GetID());
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uDiffuseTexture"), 0);
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uNormalTexture"), 1);
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uSpecularTexture"), 2);
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uEmissiveTexture"), 3);
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uMaskTexture"), 4);

    m_skMeshLocations.matColor          = m_skeletalMeshShader.GetLocationFromUniformName("uMatColors");
    m_skMeshLocations.packedMat1        = m_skeletalMeshShader.GetLocationFromUniformName("uPackedMat");
    m_skMeshLocations.matModel          = m_skeletalMeshShader.GetLocationFromUniformName("uModel");
    m_skMeshLocations.matModelNormal    = m_skeletalMeshShader.GetLocationFromUniformName("uModelNormalMatrix");
    m_skMeshLocations.matSkin           = m_skeletalMeshShader.GetLocationFromUniformName("uSkinningMatrices[0]");
}

struct litShaderPackedMat4
{
    alignas(8) Vector2 offset;
    alignas(8) Vector2 tiling;
    
    alignas(4) float  emissiveStrength;

    alignas(4) float  useDiffusetexture;
    alignas(4) float  useNormalTexture;
    alignas(4) float  useSpecularTexture;
    alignas(4) float  useEmissiveTexture;
    alignas(4) float  useMaskTexture;

    //  24 Bytes lefts
};

void MeshLitShader::SendMaterialDatas(MaterialData& material, PBRUniformsLocation const& locations) const
{
    glUniformMatrix4fv(locations.matColor, 1, GL_FALSE, &material.diffuseColor.r);

    litShaderPackedMat4 packedMat =
    {
        material.offset,
        material.tiling,
        material.emissiveStrength,
        static_cast<float>(material.diffuseTexture != nullptr),
        static_cast<float>(material.normalTexture != nullptr),
        static_cast<float>(material.specularTexture != nullptr),
        static_cast<float>(material.emissiveTexture != nullptr),
        static_cast<float>(material.maskTexture != nullptr)
    };

    glUniformMatrix4fv(locations.packedMat1, 1, GL_FALSE, &packedMat.offset.x);

    if (packedMat.useDiffusetexture)    material.diffuseTexture->GPUData->generatedTexture->BindUnit(0);
    if (packedMat.useNormalTexture)     material.normalTexture->GPUData->generatedTexture->BindUnit(1);
    if (packedMat.useSpecularTexture)   material.specularTexture->GPUData->generatedTexture->BindUnit(2);
    if (packedMat.useEmissiveTexture)   material.emissiveTexture->GPUData->generatedTexture->BindUnit(3);
    if (packedMat.useMaskTexture)       material.maskTexture->GPUData->generatedTexture->BindUnit(4);
}


void MeshLitShader::DrawMesh(const Matrix4& model, const MeshData& mesh, const MaterialData* material) const
{
    Matrix4 normalMatrix = Matrix4::Transpose(Matrix4::Inverse(model));

    glUniformMatrix4fv(m_meshLocations.matModel, 1, GL_FALSE, model.elements);
    glUniformMatrix4fv(m_meshLocations.matModelNormal, 1, GL_FALSE, normalMatrix.elements);

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
    glBindTextureUnit(2, 0);
    glBindTextureUnit(3, 0);
    glBindTextureUnit(4, 0);
}


void MeshLitShader::DrawSkeletalMesh(const Matrix4& model, const MeshData& mesh, const SkeletalData& skeleton, const MaterialData* material) const
{
    Matrix4 normalMatrix = Matrix4::Transpose(Matrix4::Inverse(model));

    glUniformMatrix4fv(m_skMeshLocations.matModel, 1, GL_FALSE, model.elements);
    glUniformMatrix4fv(m_skMeshLocations.matModelNormal, 1, GL_FALSE, normalMatrix.elements);
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
    glBindTextureUnit(2, 0);
    glBindTextureUnit(3, 0);
}
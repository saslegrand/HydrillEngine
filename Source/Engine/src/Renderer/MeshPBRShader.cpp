#include "Renderer/MeshPBRShader.hpp"

#include <glad/gl.h>

#include "ECS/MeshComponent.hpp"
#include "ECS/SkeletalMeshComponent.hpp"
#include "ECS/Transform.hpp"
#include "Renderer/GPUMeshData.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Renderer/MeshInstance.hpp"
#include "Resources/Resource/Material.hpp"
#include "Resources/Resource/Texture.hpp"
#include "EngineContext.hpp"

#pragma region PBR SHADER REGION

static const char* PBRVertexShaderStr = R"GLSL(
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

//  PACKED MAT 01 :
//        Xoffset         |      Yoffset     |      Utiling     
//        Vtiling         | emissiveStrength |    useDiffuse    
//        useNormal       |    useEmissive   |     useMask      

uniform mat3 uPackedMat01;

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

    vec2 offset = uPackedMat01[0].xy;
    vec2 tiling = vec2(uPackedMat01[0][2],uPackedMat01[1][0]);
    vs_out.uv = offset + aUV * tiling;

    vs_out.pos    = pos4.xyz / pos4.w;
    vs_out.normal = normalize(vec3(uModelNormalMatrix * vec4(aNormal, 0.0)));
    vs_out.TBN    = ComputeTBN();

    gl_Position   = uProjection * uView * pos4;
})GLSL";

static const char* skeletalPBRVertexShaderStr = R"GLSL(
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

//  PACKED MAT 01 :
//        Xoffset         |      Yoffset     |      Utiling     
//        Vtiling         | emissiveStrength |    useDiffuse    
//        useNormal       |    useEmissive   |     useMask      

uniform mat3 uPackedMat01;

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

    vec4 pos4 = uModel * vec4(totalPosition.xyz, 1.0f);

    vec2 offset = uPackedMat01[0].xy;
    vec2 tiling = vec2(uPackedMat01[0][2],uPackedMat01[1][0]);
    vs_out.uv = offset + aUV * tiling;

    vs_out.pos    = pos4.xyz / pos4.w;
    vs_out.normal = normalize(vec3(uModelNormalMatrix * vec4(localNormal, 0.0f)));
    vs_out.TBN    = ComputeTBN(localNormal);

    gl_Position   = uProjection * uView * vec4(pos4.xyz, 1.0f);
})GLSL";

static const char* PBRFragmentShaderStr = R"GLSL(
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
uniform sampler2D uEmissiveTexture;
uniform sampler2D uMetallicTexture;
uniform sampler2D uRoughnessTexture;
uniform sampler2D uAmbientOcclusionTexture;
uniform sampler2D uMaskTexture;

//  PACKED MAT COLORS :
//       Rdiffuse         |     Gdiffuse     |     Bdiffuse      |     Adiffuse      
//       Rambient         |     Gambient     |     Bambient      |       ...         
//       Rspecular        |     Gspecular    |     Bspecular     |       ...         
//       Remissive        |     Gemissive    |     Bemissive     |       ...         

uniform mat4  uMatColors;

//  PACKED MAT 01 :
//        Xoffset         |        Yoffset       |       Utiling      
//        Vtiling         |   emissiveStrength   |      useDiffuse    
//        useNormal       |      useEmissive     |       useMask      

uniform mat3 uPackedMat01;

//  PACKED MAT 02 :
//      metalness       |       roughness      |     normalFactor       
//       AOFactor       |   useMetallicTexture | useRoughnessTexture    
//     useAOTexture     |          ...         |         ...            

uniform mat3 uPackedMat02;

//  Global variables

int    cascadeCount = 4;
int    PCF = 3;
int    PCFSampleCount = (1 + 2 * PCF) * (1 + 2 * PCF);
float  PCFFactor = 1.0 / PCFSampleCount;
float  maxBias = 0.005;
float  minBias = maxBias * 0.01;

vec3 Normal;
float PI = 3.141592653;

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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float denom = NdotV * (1.0 - k) + k;
	
    return NdotV / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
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
    float bias = max(0.05 * (uDirectionalLightPlaneDistances[layer] / 500.f), 0.005);

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

vec3 computePBRShader(Light light, vec3 worldPos, vec3 normal, vec3 viewDir, vec4 PBRValues, vec3 diffuseColor, vec3 specularColor, vec3 reflection, float visibility)
{
    normal *= PBRValues[2];

    float attenuation = light.intensity;
	vec3 lightDir = vec3(0.0); 

    float isDirectional = isLowerOrEqual(light.type, 0);
    float isSpot = isGreaterOrEqual(light.type, 2);
    
	// calculate per-light radiance
	//	------------------------------
	
	// Point / Spot lights
    
    vec3 lightPosFromVertexPos = light.position - worldPos;
    lightDir += not(isDirectional) * normalize(lightPosFromVertexPos);
    float dist = length(lightPosFromVertexPos);
    attenuation *= isDirectional + not(isDirectional) * clamp(1.0 - dist*dist/(light.radius*light.radius), 0.0, 1.0);
    attenuation *= attenuation;

    float spotIntensity = not(isSpot) +  isSpot * spotLightIntensity(lightDir, light.direction, light.cutOff.x, light.cutOff.y);

    // Directional light
    lightDir += isDirectional * normalize(-light.direction);

	vec3 H = normalize(viewDir + lightDir);
	vec3 radiance = light.diffuse * attenuation;

	float NDF = DistributionGGX(normal, H, PBRValues[1]);         
    float G   = GeometrySmith(normal, viewDir, lightDir, PBRValues[1]);
    vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), reflection); 
	
	vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - PBRValues[0];	

	vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
    specular *= light.specular * specularColor;

	// add to outgoing radiance Lo
    float NdotL = max(dot(normal, lightDir), 0.0);

	return (kD * diffuseColor/ PI + specular) * radiance * NdotL * visibility * spotIntensity; 
}

vec4 GetTexturePixel(in sampler2D text, in vec2 uv, in float isUsed, in float isDefaultWhite)
{
    return isDefaultWhite * (1.0 - isUsed) + isUsed * texture(text, uv);
}

void main()
{
    bool useMaskTexture = uPackedMat01[2][2] == 1.0;
    if(useMaskTexture)
    {
        if(texture(uMaskTexture, fs_in.uv).r < 0.1f) discard;
    }

    float emissiveStrength    = uPackedMat01[1][1];
    float useDiffuseTexture   = uPackedMat01[1][2];
    float useNormalTexture    = uPackedMat01[2][0];
    float useEmissiveTexture  = uPackedMat01[2][1];

    Normal = (1.0 - useNormalTexture) * fs_in.normal + useNormalTexture * normalize(fs_in.TBN * normalize(texture(uNormalTexture, fs_in.uv).rgb * 2.0 - 1.0));

    vec4 diffuseColor  = uMatColors[0];
    vec3 ambientColor  = uMatColors[1].xyz;
    vec3 specularColor = uMatColors[2].xyz;
    vec3 emissiveColor = uMatColors[3].xyz;

    //  To linear space
    vec3 toLinear = vec3(2.2);

    vec4 diffuseTexture = GetTexturePixel(uDiffuseTexture, fs_in.uv,useDiffuseTexture, 1.0);
    diffuseTexture.rgb  = pow(diffuseTexture.rgb, toLinear);
    diffuseColor *= diffuseTexture;

    vec3 emissiveTexture = pow(GetTexturePixel(uEmissiveTexture, fs_in.uv,useEmissiveTexture, 1.0).rgb, toLinear);
    emissiveColor *= emissiveTexture * emissiveStrength;

    float metalness    = uPackedMat02[0][0];
    float roughness    = uPackedMat02[0][1];
    float normalFactor = uPackedMat02[0][2];
    float AOFactor     = uPackedMat02[1][0];
    float useMetallicTexture  = uPackedMat02[1][1];
    float useRoughnessTexture = uPackedMat02[1][2];
    float useAOTexture        = uPackedMat02[2][0];

    vec4 pbrValue = vec4(
        GetTexturePixel(uMetallicTexture, fs_in.uv,useMetallicTexture, 1.0).r   * metalness,
        GetTexturePixel(uRoughnessTexture, fs_in.uv,useRoughnessTexture, 1.0).r * roughness,
        normalFactor,
        GetTexturePixel(uAmbientOcclusionTexture, fs_in.uv,useAOTexture, 1.0).r * AOFactor
    );

    vec3 viewDir = normalize(uViewPos - fs_in.pos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0,  diffuseColor.rgb , pbrValue[0]);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    vec3 lightambient = vec3(0.0,0.0,0.0); 
    for (int i = 0; i < uNumLights; ++i)
    {
        if(!uLights[i].enabled) continue;

        float visibility = computeVisibility(uLights[i], i);
        Lo += computePBRShader(uLights[i], fs_in.pos, Normal, viewDir,  pbrValue, diffuseColor.rgb, specularColor, F0, visibility);
        lightambient += uLights[i].ambient;
    }

    ambientColor *= lightambient;

    vec3 shadedColor = vec3(0.03) * diffuseColor.rgb * pbrValue[3];
    shadedColor += Lo;

    // Apply light color
    oColor = vec4(shadedColor + emissiveColor + ambientColor, diffuseColor.a);
})GLSL";

#pragma endregion

void MeshPBRShader::Initialize()
{
    m_meshShader.Generate(PBRVertexShaderStr, PBRFragmentShaderStr);
    m_skeletalMeshShader.Generate(skeletalPBRVertexShaderStr, PBRFragmentShaderStr);

    glUseProgram(m_meshShader.GetID());
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uDiffuseTexture"), 0);
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uNormalTexture"), 1);
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uEmissiveTexture"), 2);
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uMetallicTexture"), 3);
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uRoughnessTexture"), 4);
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uAmbientOcclusionTexture"), 5);
    glUniform1i(glGetUniformLocation(m_meshShader.GetID(), "uMaskTexture"), 6);

    m_meshLocations.matColor        = m_meshShader.GetLocationFromUniformName("uMatColors");
    m_meshLocations.packedMat1      = m_meshShader.GetLocationFromUniformName("uPackedMat01");
    m_meshLocations.packedMat2      = m_meshShader.GetLocationFromUniformName("uPackedMat02");
    m_meshLocations.matModel        = m_meshShader.GetLocationFromUniformName("uModel");
    m_meshLocations.matModelNormal  = m_meshShader.GetLocationFromUniformName("uModelNormalMatrix");

    glUseProgram(m_skeletalMeshShader.GetID());
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uDiffuseTexture"), 0);
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uNormalTexture"), 1);
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uEmissiveTexture"), 2);
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uMetallicTexture"), 3);
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uRoughnessTexture"), 4);
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uAmbientOcclusionTexture"), 5);
    glUniform1i(glGetUniformLocation(m_skeletalMeshShader.GetID(), "uMaskTexture"), 6);

    m_skMeshLocations.matColor          = m_skeletalMeshShader.GetLocationFromUniformName("uMatColors");
    m_skMeshLocations.packedMat1        = m_skeletalMeshShader.GetLocationFromUniformName("uPackedMat01");
    m_skMeshLocations.packedMat2        = m_skeletalMeshShader.GetLocationFromUniformName("uPackedMat02");
    m_skMeshLocations.matModel          = m_skeletalMeshShader.GetLocationFromUniformName("uModel");
    m_skMeshLocations.matModelNormal    = m_skeletalMeshShader.GetLocationFromUniformName("uModelNormalMatrix");
    m_skMeshLocations.matSkin           = m_skeletalMeshShader.GetLocationFromUniformName("uSkinningMatrices[0]");
}

struct pbrShaderPackedMat3_01
{
    alignas(8) Vector2 offset;
    alignas(8) Vector2 tiling;

    alignas(4) float  emissiveStrength;
    alignas(4) float  useDiffuseTexture;

    alignas(4) float  useNormalTexture;
    alignas(4) float  useEmissiveTexture;
    alignas(4) float  useMaskTexture;
};

struct pbrShaderPackedMat3_02
{
    alignas(4) float  metalness;
    alignas(4) float  roughness;
    alignas(4) float  normalFactor;
    alignas(4) float  AOFactor;

    alignas(4) float  useMetallicTexture;
    alignas(4) float  useRoughnessTexture;
    alignas(4) float  useAOTexture;

    //  8 Bytes lefts
};

void MeshPBRShader::SendMaterialDatas(MaterialData& material, PBRUniformsLocation const& locations) const
{
    glUniformMatrix4fv(locations.matColor, 1, GL_FALSE, &material.diffuseColor.r);

    pbrShaderPackedMat3_01 packedMat01 =
    {
        material.offset,
        material.tiling,
        material.emissiveStrength,
        static_cast<float>(material.diffuseTexture != nullptr),
        static_cast<float>(material.normalTexture != nullptr),
        static_cast<float>(material.emissiveTexture != nullptr),
        static_cast<float>(material.maskTexture != nullptr)
    };

    pbrShaderPackedMat3_02 packedMat02 =
    {
        material.metalness,
        material.roughness,
        material.normalFactor,
        material.aoFactor,

        static_cast<float>(material.metallicTexture != nullptr),
        static_cast<float>(material.roughnessTexture != nullptr),
        static_cast<float>(material.aoTexture != nullptr)
    };

    glUniformMatrix3fv(locations.packedMat1, 1, GL_FALSE, &packedMat01.offset.x);
    glUniformMatrix3fv(locations.packedMat2, 1, GL_FALSE, &packedMat02.metalness);

    if (packedMat01.useDiffuseTexture)     material.diffuseTexture->GPUData->generatedTexture->BindUnit(0);
    if (packedMat01.useNormalTexture)      material.normalTexture->GPUData->generatedTexture->BindUnit(1);
    if (packedMat01.useEmissiveTexture)    material.emissiveTexture->GPUData->generatedTexture->BindUnit(2);
    if (packedMat01.useMaskTexture)        material.maskTexture->GPUData->generatedTexture->BindUnit(6);

    if (packedMat02.useMetallicTexture)    material.metallicTexture->GPUData->generatedTexture->BindUnit(3);
    if (packedMat02.useRoughnessTexture)   material.roughnessTexture->GPUData->generatedTexture->BindUnit(4);
    if (packedMat02.useAOTexture)          material.aoTexture->GPUData->generatedTexture->BindUnit(5);
}


void MeshPBRShader::DrawMesh(const Matrix4& model, const MeshData& mesh, const MaterialData* material) const
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
    glBindTextureUnit(5, 0);
    glBindTextureUnit(6, 0);
}

void MeshPBRShader::DrawSkeletalMesh(const Matrix4& model, const MeshData& mesh, const SkeletalData& skeleton, const MaterialData* material) const
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
    glBindTextureUnit(4, 0);
    glBindTextureUnit(5, 0);
    glBindTextureUnit(6, 0);
}

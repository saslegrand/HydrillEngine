#include "Renderer/ParticleRenderPipeline.hpp"

#include <glad/gl.h>

#include "Core/Logger.hpp"
#include "ECS/ParticleComponent.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/SystemManager.hpp"
#include "Renderer/RenderObjects/Camera.hpp"
#include "Renderer/RenderSystem.hpp"
#include "ECS/TransformData.hpp"
#include "EngineContext.hpp"

#define MAX_PARTICLE_GPU static_cast<unsigned int>(10000)


static const char* particleVertexShaderStr = R"GLSL(
	#version 460 core

	// Attributes

	layout(location = 0)  in vec3 aPosition;
	layout(location = 1)  in vec2 aUV;
	layout(location = 2)  in mat4 aModel;

	layout(location = 0) flat out int aIndex;

	//  Uniform Buffers

	layout (std140, binding = 0) uniform UBO_camera
	{
		mat4 uProjection;
		mat4 uView;
		vec3 uViewPos;
	};

	// Varyings

	out VS_OUT
	{
		vec2 uv;
	}vs_out;

	void main()
	{
		vs_out.uv = aUV;

        aIndex = gl_InstanceID;

		gl_Position = uProjection * uView * aModel * vec4(aPosition, 1.0);
    }
)GLSL";

static const char* particleFragmentShaderStr = R"GLSL(
#version 460 core

	layout(location = 0) flat in int aIndex;

    //  Outputs

	layout(location = 0) out vec4 oColor;

    //  Structures

    struct Particle
    {
        float diffuseTextureLayer;
        float emissiveTextureLayer;
        float blendTexture01Layer;
        float blendTexture02Layer;
        float blendTexture03Layer;
        float blendTexture04Layer;

        float diffuseTextureWeight;
        float emissiveTextureWeight;
        float blendTexture01Weight;
        float blendTexture02Weight;
        float blendTexture03Weight;
        float blendTexture04Weight;

        vec4 diffuseUVs;
        vec4 emissiveUVs;
        vec4 blend01UVs;
        vec4 blend02UVs;
        vec4 blend03UVs;
        vec4 blend04UVs;

        vec4  color;
        vec3  emissiveColor;
        float emissiveStrength;
    };

	// Varyings

	in VS_OUT
	{
		vec2 uv;
	} fs_in;

	//  Uniform Buffers

	layout (std430, binding = 3) buffer UBO_Instances
	{
		Particle uParticles[];
	};

	//  Uniforms

    layout (binding = 0) uniform sampler2DArray uTextureArray;

	//  Functions

    float isNotEqual(float x, float y) 
    {
      return abs(sign(x - y));
    }

    vec4 GetPixelFromTexture(in float index, in vec4 texCoord, in float weight, in float isDefaultWhite)
    {
        float useTexture = isNotEqual(index, -1.0f);

        float u = texCoord.x + fs_in.uv.x * texCoord.z;
        float v = texCoord.y + fs_in.uv.y * texCoord.w;
        vec3 uvIndex = vec3(u,v, index);
        return isDefaultWhite * (1.0 - useTexture) + useTexture * texture(uTextureArray, uvIndex) * weight;
    }

	void main()
	{
        Particle p = uParticles[aIndex];

        float blend = 1.0f;
		blend *= GetPixelFromTexture(p.blendTexture01Layer, p.blend01UVs, p.blendTexture01Weight, 1.0f).r;
		blend *= GetPixelFromTexture(p.blendTexture02Layer, p.blend02UVs, p.blendTexture02Weight, 1.0f).r;
		blend *= GetPixelFromTexture(p.blendTexture03Layer, p.blend03UVs, p.blendTexture03Weight, 1.0f).r;
		blend *= GetPixelFromTexture(p.blendTexture04Layer, p.blend04UVs, p.blendTexture04Weight, 1.0f).r;

        if(blend < 0.01f) discard;

		vec4 color = p.color;
		color *= GetPixelFromTexture(p.diffuseTextureLayer, p.diffuseUVs, p.diffuseTextureWeight, 1.0f);

        vec3 emissive = GetPixelFromTexture(p.emissiveTextureLayer, p.emissiveUVs, p.emissiveTextureWeight, 1.0f).rgb * p.emissiveColor * p.emissiveStrength;

		color.rgb += emissive;
        color.a *= blend;

        oColor = color;
	}
)GLSL";

#pragma endregion


ParticleRenderPipeline::ParticleRenderPipeline()
{
}

void ParticleRenderPipeline::Initialize()
{
    m_shader.Generate(particleVertexShaderStr, particleFragmentShaderStr);

    GLuint vao = m_VAO.GetID();

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

        GLuint vbo = m_VBO.GetID();

        // Allocate vertex buffer storage (empty)
        glNamedBufferStorage(vbo, sizeof(vertices), vertices, GL_MAP_READ_BIT);
        // Link array and buffer (binding 0)
        glVertexArrayVertexBuffer(vao, 0, vbo, 0, 5 * sizeof(float));
    }


    {
        GLuint vbo = m_VBOParticleMatrices.GetID();

        glNamedBufferData(vbo, MAX_PARTICLE_GPU * sizeof(Matrix4), NULL, GL_DYNAMIC_DRAW);

        glVertexArrayVertexBuffer(vao, 1, vbo, 0, sizeof(Matrix4));

        //  Texture indexes
        glEnableVertexArrayAttrib(vao, 2 + 0);
        glEnableVertexArrayAttrib(vao, 2 + 1);
        glEnableVertexArrayAttrib(vao, 2 + 2);
        glEnableVertexArrayAttrib(vao, 2 + 3);

        glVertexArrayAttribBinding(vao, 2 + 0, 1);
        glVertexArrayAttribBinding(vao, 2 + 1, 1);
        glVertexArrayAttribBinding(vao, 2 + 2, 1);
        glVertexArrayAttribBinding(vao, 2 + 3, 1);

        glVertexArrayAttribFormat(vao, 2 + 0, 4, GL_FLOAT, GL_FALSE, offsetof(Matrix4, columns[0]));
        glVertexArrayAttribFormat(vao, 2 + 1, 4, GL_FLOAT, GL_FALSE, offsetof(Matrix4, columns[1]));
        glVertexArrayAttribFormat(vao, 2 + 2, 4, GL_FLOAT, GL_FALSE, offsetof(Matrix4, columns[2]));
        glVertexArrayAttribFormat(vao, 2 + 3, 4, GL_FLOAT, GL_FALSE, offsetof(Matrix4, columns[3]));

        glVertexArrayBindingDivisor(vao, 1, 1);
    }

    glCreateBuffers(1, &m_indirectBuffer);

    //  Particle instances SSBO
    glGenBuffers(1, &m_particlesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particlesSSBO);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, m_particlesSSBO, 0, sizeof(ParticleInstanceGPU) * MAX_PARTICLE_GPU);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleInstanceGPU) * MAX_PARTICLE_GPU, nullptr, GL_DYNAMIC_DRAW);

    GLuint block_index = glGetProgramResourceIndex(m_shader.GetID(), GL_SHADER_STORAGE_BLOCK, "UBO_Instances");
    glShaderStorageBlockBinding(m_shader.GetID(), block_index, 3);
}

void ParticleRenderPipeline::ClearParticleInstances()
{
    m_particleComponents.clear();
}


void ParticleRenderPipeline::AddParticleInstance(ParticleComponent* particleComponent)
{
    if (particleComponent == nullptr) return;

    auto foundComp = std::find(m_particleComponents.begin(), m_particleComponents.end(), particleComponent);

    if (foundComp != m_particleComponents.end())
    {
        Logger::Warning("ParticleRenderPipeline - Can't register one particle component instance multiple times");
        return;
    }

    m_particleComponents.emplace_back(particleComponent);
}


void ParticleRenderPipeline::RemoveParticleInstance(ParticleComponent* particleComponent)
{
    if (particleComponent == nullptr || m_particleComponents.empty()) return;

    auto foundComp = std::find(m_particleComponents.begin(), m_particleComponents.end(), particleComponent);

    if (foundComp == m_particleComponents.end())
    {
        return;
    }

    m_particleComponents.erase(foundComp);
}

typedef struct {
    unsigned int vertexCount;
    unsigned int instanceCount;
    unsigned int firstIndex;
    unsigned int baseInstance;
} DrawArraysIndirectCommand;

float GetParticleTextureLayer(const ParticleTexture& text)
{
    return text.texture ? text.texture->GPUData->arrayIndex : -1.f;
}

void ParticleRenderPipeline::SendTextureDatas(ParticleInstanceGPU& dst, const ParticleEmitter& src)
{
    dst.diffuseTextureWeight = src.diffuseTexture.weight;
    dst.emissiveTextureWeight = src.emissiveTexture.weight;
    dst.blendTexture01Weight = src.blendTexture01.weight;
    dst.blendTexture02Weight = src.blendTexture02.weight;
    dst.blendTexture03Weight = src.blendTexture03.weight;
    dst.blendTexture04Weight = src.blendTexture04.weight;

    dst.diffuseTextureLayer = GetParticleTextureLayer(src.diffuseTexture);
    dst.emissiveTextureLayer = GetParticleTextureLayer(src.emissiveTexture);
    dst.blendTexture01Layer = GetParticleTextureLayer(src.blendTexture01);
    dst.blendTexture02Layer = GetParticleTextureLayer(src.blendTexture02);
    dst.blendTexture03Layer = GetParticleTextureLayer(src.blendTexture03);
    dst.blendTexture04Layer = GetParticleTextureLayer(src.blendTexture04);
}

void ParticleRenderPipeline::SendInstanceDatas(ParticleInstanceGPU& dst, const ParticleInstance& src)
{
    dst.color = src.color;
    dst.emissiveColor = src.emissiveColor;
    dst.emissiveStrength = src.emissiveStrength;

    dst.diffuseTextureUV = src.diffuseUV;
    dst.emissiveTextureUV = src.emissiveUV;
    dst.blendTextureUV_01 = src.BlendUV_01;
    dst.blendTextureUV_02 = src.BlendUV_02;
    dst.blendTextureUV_03 = src.BlendUV_03;
    dst.blendTextureUV_04 = src.BlendUV_04;
}

void ParticleRenderPipeline::SetupCommand(unsigned int instanceCount)
{
    DrawArraysIndirectCommand drawCommand;

    drawCommand.vertexCount = 6;
    drawCommand.firstIndex = 0;
    drawCommand.instanceCount = instanceCount;
    drawCommand.baseInstance = 0;

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_indirectBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectCommand), &drawCommand, GL_DYNAMIC_DRAW);
}

void ParticleRenderPipeline::GenerateOpaqueInstancesFromEmitter(const ParticleEmitter& emitter)
{
    unsigned int size = 0;

    if (emitter.billboard)
    {
        const Camera* renderingCamera = SystemManager::GetCameraSystem().renderingCamera;

        for (const ParticleInstance& instance : emitter.instances)
        {
            if (!instance.IsAlive() || size >= MAX_PARTICLE_GPU) continue;

            ParticleInstanceGPU& GPUData = m_opaqueParticleInstanceGPU.emplace_back();

            SendTextureDatas(GPUData, emitter);
            SendInstanceDatas(GPUData, instance);

            Matrix4 rotation = Quaternion::ToMatrix(Quaternion::FromAxisAngle(m_cameraForward, instance.billboardRotation) * m_billboardRotation );

            m_opaqueParticleInstanceModels.emplace_back
            (
                Matrix4::Translate(instance.GetPosition()) *
                rotation *
                Matrix4::Scale(instance.GetScale())
            );

            size++;
        }
    }
    else
    {
        for (const ParticleInstance& instance : emitter.instances)
        {
            if (!instance.IsAlive() || size >= MAX_PARTICLE_GPU) continue;

            ParticleInstanceGPU& GPUData = m_opaqueParticleInstanceGPU.emplace_back();

            SendTextureDatas(GPUData, emitter);
            SendInstanceDatas(GPUData, instance);

            m_opaqueParticleInstanceModels.emplace_back(instance.globalTransformModel);
            size++;
        }
    }
}

void ParticleRenderPipeline::GenerateAllBlendedInstances(const std::vector<BlendParticleInstance>& instances)
{
    unsigned int size = 0;

    const Camera* renderingCamera = SystemManager::GetCameraSystem().renderingCamera;

    for (const BlendParticleInstance& instance : instances)
    {
        const ParticleInstance& particle = *instance.particle;

        if (!particle.IsAlive() || size >= MAX_PARTICLE_GPU) continue;

        ParticleInstanceGPU& GPUData = m_blendedParticleInstanceGPU.emplace_back();
        GPUData.diffuseTextureLayer = instance.diffuseTextureLayer;
        GPUData.emissiveTextureLayer = instance.emissiveTextureLayer;
        GPUData.blendTexture01Layer = instance.blendTexture01Layer;
        GPUData.blendTexture02Layer = instance.blendTexture02Layer;
        GPUData.blendTexture03Layer = instance.blendTexture03Layer;
        GPUData.blendTexture04Layer = instance.blendTexture04Layer;

        GPUData.diffuseTextureWeight = instance.diffuseTextureWeight;
        GPUData.emissiveTextureWeight = instance.emissiveTextureWeight;
        GPUData.blendTexture01Weight = instance.blendTexture01Weight;
        GPUData.blendTexture02Weight = instance.blendTexture02Weight;
        GPUData.blendTexture03Weight = instance.blendTexture03Weight;
        GPUData.blendTexture04Weight = instance.blendTexture04Weight;

        GPUData.diffuseTextureUV = instance.particle->diffuseUV;
        GPUData.blendTextureUV_01 = instance.particle->BlendUV_01;
        GPUData.blendTextureUV_02 = instance.particle->BlendUV_02;
        GPUData.blendTextureUV_03 = instance.particle->BlendUV_03;
        GPUData.blendTextureUV_04 = instance.particle->BlendUV_04;

        GPUData.color = instance.particle->color;
        GPUData.emissiveColor = instance.particle->emissiveColor;
        GPUData.emissiveStrength = instance.particle->emissiveStrength;

        if (instance.billboard)
        {
            Matrix4 rotation = Quaternion::ToMatrix(Quaternion::FromAxisAngle(m_cameraForward, particle.billboardRotation) * m_billboardRotation);

            m_blendedParticleInstanceModels.emplace_back
            (
                Matrix4::Translate(particle.GetPosition()) *
                rotation *
                Matrix4::Scale(particle.GetScale())
            );
        }
        else
        {
            m_blendedParticleInstanceModels.emplace_back(particle.globalTransformModel);
        }
        size++;
    }
}


void ParticleRenderPipeline::PreRender()
{
    //  Clear but keep capacity
    m_opaqueParticleInstanceGPU.clear();
    m_blendedParticleInstanceGPU.clear();
    m_opaqueParticleInstanceModels.clear();
    m_blendedParticleInstanceModels.clear();

    //  Calculate billboard rotation matrix just once
    m_billboardRotation = SystemManager::GetCameraSystem().renderingCamera->GetBillboardRotation();
    m_cameraForward = -TransformData::Forward(SystemManager::GetCameraSystem().renderingCamera->GetTransformMatrix());

    std::vector<const ParticleEmitter*> blendEmitters;

    for (const ParticleComponent* particle : m_particleComponents)
    {
        for (const ParticleEmitter& emitter : particle->emitters)
        {
            if (emitter.state == ParticleEmitter::EmitterState::Inactive || emitter.state == ParticleEmitter::EmitterState::Finished) continue;

            if (emitter.blending)
            {
                blendEmitters.emplace_back(&emitter);
                continue;
            }
            GenerateOpaqueInstancesFromEmitter(emitter);
        }
    }

    if (!blendEmitters.empty())
    {
        Vector3 camPos = SystemManager::GetCameraSystem().renderingCamera->GetPosition();

        //  Stock all particles with blending on in a list of specific particle instances which will be sorted later
        for (const ParticleEmitter* emitter : blendEmitters)
        {
            for (const ParticleInstance& instance : emitter->instances)
            {
                BlendParticleInstance& blendParticle = m_sortedBlendedInstances.emplace_back();

                //  Needed for sorting
                blendParticle.distanceFromCamera = (instance.GetPosition() - camPos).SquaredMagnitude();

                blendParticle.particle = &instance;
                blendParticle.billboard = emitter->billboard;

                blendParticle.diffuseTextureWeight = emitter->diffuseTexture.weight;
                blendParticle.emissiveTextureWeight = emitter->emissiveTexture.weight;
                blendParticle.blendTexture01Weight = emitter->blendTexture01.weight;
                blendParticle.blendTexture02Weight = emitter->blendTexture02.weight;
                blendParticle.blendTexture03Weight = emitter->blendTexture03.weight;
                blendParticle.blendTexture04Weight = emitter->blendTexture04.weight;

                blendParticle.diffuseTextureLayer = GetParticleTextureLayer(emitter->diffuseTexture);
                blendParticle.emissiveTextureLayer = GetParticleTextureLayer(emitter->emissiveTexture);
                blendParticle.blendTexture01Layer = GetParticleTextureLayer(emitter->blendTexture01);
                blendParticle.blendTexture02Layer = GetParticleTextureLayer(emitter->blendTexture02);
                blendParticle.blendTexture03Layer = GetParticleTextureLayer(emitter->blendTexture03);
                blendParticle.blendTexture04Layer = GetParticleTextureLayer(emitter->blendTexture04);
            }
        }

        //  Sort the particles depending on their distance with the current rendering camera
        std::sort(m_sortedBlendedInstances.begin(), m_sortedBlendedInstances.end(), [](const BlendParticleInstance& a, const BlendParticleInstance& b)->bool
            {
                return a.distanceFromCamera > b.distanceFromCamera;
            }
        );

        GenerateAllBlendedInstances(m_sortedBlendedInstances);
    }
}

void ParticleRenderPipeline::Render()
{
    glBindVertexArray(m_VAO.GetID());
    glDisable(GL_CULL_FACE);

    GLTextureArray& textureArray = SystemManager::GetRenderSystem().GPUWrapper.GeneratedTextureArray;

    m_shader.Bind();
    textureArray.BindUnit(0);

    size_t opaqueSize = m_opaqueParticleInstanceGPU.size();
    if (opaqueSize > 0)
    {
        //	Send datas to buffers
        glNamedBufferSubData(m_particlesSSBO, 0, opaqueSize * sizeof(ParticleInstanceGPU), m_opaqueParticleInstanceGPU.data());
        glNamedBufferSubData(m_VBOParticleMatrices.GetID(), 0, opaqueSize * sizeof(Matrix4), m_opaqueParticleInstanceModels.data());

        SetupCommand(static_cast<unsigned int>(opaqueSize));

        glMultiDrawArraysIndirect(GL_TRIANGLES, (GLvoid*)0, 1, 0);

        //  Clear buffers after use
        glNamedBufferSubData(m_VBOParticleMatrices.GetID(), 0, MAX_PARTICLE_GPU * sizeof(Matrix4), nullptr);
        glNamedBufferSubData(m_particlesSSBO, 0, MAX_PARTICLE_GPU * sizeof(ParticleInstanceGPU), nullptr);
    }

    //  Blended particles

    size_t blendedSize = m_blendedParticleInstanceGPU.size();
    if (blendedSize > 0)
    {
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //	Send datas to buffers
        glNamedBufferSubData(m_particlesSSBO, 0, blendedSize * sizeof(ParticleInstanceGPU), m_blendedParticleInstanceGPU.data());
        glNamedBufferSubData(m_VBOParticleMatrices.GetID(), 0, blendedSize * sizeof(Matrix4), m_blendedParticleInstanceModels.data());

        SetupCommand(static_cast<unsigned int>(blendedSize));

        glMultiDrawArraysIndirect(GL_TRIANGLES, (GLvoid*)0, 1, 0);

        //  Clear buffers after use
        glNamedBufferSubData(m_VBOParticleMatrices.GetID(), 0, MAX_PARTICLE_GPU * sizeof(Matrix4), nullptr);
        glNamedBufferSubData(m_particlesSSBO, 0, MAX_PARTICLE_GPU * sizeof(ParticleInstanceGPU), nullptr);

        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        //  Clear sorted instances vector without resetting capacity to 0
        m_sortedBlendedInstances.clear();
    }

    textureArray.UnbindUnit(0);
    m_shader.Unbind();

    glEnable(GL_CULL_FACE);
    glBindVertexArray(0);
}

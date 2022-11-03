#include "Renderer/PostProcess.hpp"

#include <glad/gl.h>

#include "Maths/Vector3.hpp"
#include "Core/Time.hpp"
#include "Core/Logger.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/SystemManager.hpp"
#include "EngineContext.hpp"
#include "Renderer/RenderSystem.hpp"

#pragma region PostProcessDefaultVertextShader
static const char* postProcessDefaultVertexShaderStr = R"GLSL(
#version 450 core

//  Attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;

//  Varyings
out vec2 vUV;

void main()
{
    vUV = aUV;
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
})GLSL";
#pragma endregion

#pragma region HDRFragmentShader
static const char* hdrFragmentShader = R"GLSL(
#version 450 core

layout (location = 0) out vec3 oColor;
layout (location = 1) out vec3 oBloomColor;

//  Varyings
in vec2 vUV;

//  Uniforms
uniform sampler2D uScreenTexture;

uniform float uExposure;
uniform float uBloomThreshold;
uniform bool  uBloom;

void main()
{
	vec3 fragment = texture(uScreenTexture, vUV).rgb;

	vec3 toneMapped = vec3(1.0f) - exp(-fragment * uExposure);

	oColor = pow(toneMapped, vec3(1.0f / 2.2)); 

    if(uBloom && (fragment.r > uBloomThreshold || fragment.g > uBloomThreshold || fragment.b > uBloomThreshold)) oBloomColor = oColor;

    else oBloomColor = vec3(0.0f,0.0f,0.0f);

})GLSL";
#pragma endregion


#pragma region LuminanceFragmentShader
static const char* luminanceFragmentShader = R"GLSL(
#version 450 core

layout (location = 0) out vec3 oColor;

//  Varyings
in vec2 vUV;

//  Uniforms
uniform sampler2D uScreenTexture;

void main()
{
	vec3 fragment = texture(uScreenTexture, vUV).rgb;
	oColor = fragment * vec3(0.21,0.71,0.07);

})GLSL";
#pragma endregion

#pragma region BloomFragmentShader
static const char* bloomFragmentShader = R"GLSL(
#version 450 core

//  Varyings
in vec2 vUV;

//  Uniforms
uniform sampler2D uColorTexture;
uniform sampler2D uBloomTexture;

uniform bool uBloom;

//  Shader outputs
out vec3 oColor;

void main()
{
	vec3 fragment = texture(uColorTexture, vUV).rgb;
	fragment += float(uBloom) * texture(uBloomTexture, vUV).rgb;

	oColor = fragment; 
})GLSL";
#pragma endregion

#pragma region BloomBlurFragmentShader
static const char* bloomBlurFragmentShaderStr = R"GLSL(
#version 450 core

//  Varyings
in vec2 vUV;

//  Uniforms
uniform sampler2D uScreenTexture;

uniform bool uHorizontal;
uniform float uweight[5] = float[] (0.227025f, 0.1945946f,0.1216216f,0.054054f,0.016216f);

//  Shader outputs
out vec3 oColor;

void main()
{
	vec2 textureOffset = 1.0f / textureSize(uScreenTexture, 0);
	vec3 fragment = texture(uScreenTexture, vUV).rgb * uweight[0];

	if(uHorizontal)
	{
		for( int i = 0; i < 5; i++)
		{
			fragment += texture(uScreenTexture, vUV + vec2(textureOffset.x * i, 0.0)).rgb * uweight[i];
			fragment += texture(uScreenTexture, vUV - vec2(textureOffset.x * i, 0.0)).rgb * uweight[i];
		}
	}
	else
	{
		for( int i = 0; i < 5; i++)
		{
			fragment += texture(uScreenTexture, vUV + vec2(0.0, textureOffset.y * i)).rgb * uweight[i];
			fragment += texture(uScreenTexture, vUV - vec2(0.0, textureOffset.y * i)).rgb * uweight[i];
		}
	}
	oColor = fragment; 
})GLSL";
#pragma endregion

void PostProcess::Initialize()
{
	// Save current bound framebuffer
	GLint previousFramebuffer;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousFramebuffer);

	//	Set up a special frame buffer for the post process so we don't use the common FrameBuffer class
	InitializeCaptureProcess();
	InitializeLuminanceProcess();
	InitializeBloomProcess();

	//	Rebind saved framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);

}

void PostProcess::InitializeCaptureProcess()
{
	//	Shaders initializations

	m_hdrShader.Generate(postProcessDefaultVertexShaderStr, hdrFragmentShader);

	m_hdrShader.Bind();
	glUniform1i(glGetUniformLocation(m_hdrShader.GetID(), "uScreenTexture"), 0);

	GLsizei x = static_cast<GLsizei>(m_dimensions.x);
	GLsizei y = static_cast<GLsizei>(m_dimensions.y);

	//	Generate and bind the FrameBufferObject
	glGenFramebuffers(1, &m_captureFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFrameBuffer);

	//	Generate a multisample texture to render into then bind it to the FrameBufferObject
	{
		glGenTextures(1, &m_capturedMSTexture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_capturedMSTexture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB16F, x, y, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_capturedMSTexture, 0);
	}

	//	Generate a render buffer, we need it for depth & stencil testing
	{
		glGenRenderbuffers(1, &m_captureRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_captureRenderBuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, x, y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_captureRenderBuffer);
	}

	//	Then check if the frame buffer is complete
	if (glCheckNamedFramebufferStatus(m_captureFrameBuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::Error("PostProcess - FrameBuffer - Uncomplete framebuffer");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	Intermediate frame buffer
	glGenFramebuffers(1, &m_intermediateCaptureFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateCaptureFrameBuffer);

	//	Generate finale texture
	{
		glGenTextures(1, &m_capturedTexture);
		glBindTexture(GL_TEXTURE_2D, m_capturedTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_capturedTexture, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	Then check if the frame buffer is complete
	if (glCheckNamedFramebufferStatus(m_intermediateCaptureFrameBuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::Error("PostProcess - Intermediate FrameBuffer - Uncomplete framebuffer");
	}
}

void PostProcess::InitializeLuminanceProcess()
{
	m_luminanceShader.Generate(postProcessDefaultVertexShaderStr, luminanceFragmentShader);
	m_luminanceShader.Bind();

	glUniform1i(glGetUniformLocation(m_luminanceShader.GetID(), "uScreenTexture"), 0);

	GLsizei x = static_cast<GLsizei>(m_dimensions.x);
	GLsizei y = static_cast<GLsizei>(m_dimensions.y);

	{
		//	Intermediate frame buffer
		glGenFramebuffers(1, &m_brightnessFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_brightnessFrameBuffer);

		//	Generate finale texture
		{
			glGenTextures(1, &m_brightnessTexture);
			glBindTexture(GL_TEXTURE_2D, m_brightnessTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, static_cast<GLsizei>(m_BrightnessTextureResolution), static_cast<GLsizei>(m_BrightnessTextureResolution / m_ratio), 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brightnessTexture, 0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//	Then check if the frame buffer is complete
		if (glCheckNamedFramebufferStatus(m_brightnessFrameBuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Logger::Error("PostProcess - Intermediate FrameBuffer - Uncomplete framebuffer");
		}
	}

	m_1x1BrightnessTextureMipMapLevel = Compute1x1MipMap(
		m_BrightnessTextureResolution,
		static_cast<int>(m_BrightnessTextureResolution / m_ratio)
	);
}

void PostProcess::InitializeBloomProcess()
{
	//	Shaders initializations

	m_blurShader.Generate(postProcessDefaultVertexShaderStr, bloomBlurFragmentShaderStr);
	m_finalShader.Generate(postProcessDefaultVertexShaderStr, bloomFragmentShader);

	m_finalShader.Bind();
	glUniform1i(glGetUniformLocation(m_finalShader.GetID(), "uColorTexture"), 0);
	glUniform1i(glGetUniformLocation(m_finalShader.GetID(), "uBloomTexture"), 1);

	m_blurShader.Bind();
	glUniform1i(glGetUniformLocation(m_blurShader.GetID(), "uScreenTexture"), 0);

	GLsizei x = static_cast<GLsizei>(m_dimensions.x);
	GLsizei y = static_cast<GLsizei>(m_dimensions.y);

	{
		//	Intermediate frame buffer
		glGenFramebuffers(1, &m_bloomFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_bloomFrameBuffer);

		//	Generate finale texture
		{
			glGenTextures(1, &m_colorTexture);
			glBindTexture(GL_TEXTURE_2D, m_colorTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);

			glGenTextures(1, &m_bloomTexture);
			glBindTexture(GL_TEXTURE_2D, m_bloomTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_bloomTexture, 0);
		}

		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//	Then check if the frame buffer is complete
		if (glCheckNamedFramebufferStatus(m_bloomFrameBuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Logger::Error("PostProcess - Intermediate FrameBuffer - Uncomplete framebuffer");
		}
	}

	{
		// ping-pong framebuffer for blurring
		glGenFramebuffers(2, m_pingpongFrameBuffer);
		glGenTextures(2, m_pingpongTexture);

		for (size_t i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFrameBuffer[i]);
			glBindTexture(GL_TEXTURE_2D, m_pingpongTexture[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongTexture[i], 0);

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// also check if framebuffers are complete (no need for depth buffer)
			if (glCheckNamedFramebufferStatus(m_pingpongFrameBuffer[i], GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				Logger::Error("PostProcess - Pingpong FrameBuffer - Uncomplete framebuffer");
			}
		}
	}
}


unsigned int PostProcess::Compute1x1MipMap(int width, int length)
{
	//	Compute 1x1 mipmap level dependign on size
	unsigned int maxSize = static_cast<int>(Maths::Max(width, length));
	return static_cast<unsigned int>(log2(maxSize));
}


void PostProcess::BeginFrameBufferDraw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFrameBuffer);
}

void PostProcess::EndFrameBufferDraw()
{
	GLint x = static_cast<GLint>(m_dimensions.x);
	GLint y = static_cast<GLint>(m_dimensions.y);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_captureFrameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_intermediateCaptureFrameBuffer);
	glBlitFramebuffer(0, 0, x, y, 0, 0, x, y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void PostProcess::ComputeBrightnessFromTexture(unsigned int texture)
{
	//	Set brightness texture which has a low resolution to avoid performance more performance lost with "glGenerateMipmap" 
	{
		glViewport(0, 0, static_cast<GLsizei>(m_BrightnessTextureResolution), static_cast<GLsizei>(m_BrightnessTextureResolution / m_ratio));
		glBindFramebuffer(GL_FRAMEBUFFER, m_brightnessFrameBuffer);

		m_luminanceShader.Bind();

		glBindTextureUnit(0, texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		m_luminanceShader.Unbind();
	}

	//	Then compress the texture to 1x1
	glBindTexture(GL_TEXTURE_2D, m_brightnessTexture);
	glGenerateMipmap(GL_TEXTURE_2D);

	//	 Compute average color value of the captured texture
	Vector3 luminescence;
	glGetTexImage(GL_TEXTURE_2D, m_1x1BrightnessTextureMipMapLevel, GL_RGB, GL_FLOAT, &luminescence);
	
	SystemManager::GetCameraSystem().renderingCamera->SetCurrentBrightness(luminescence.x + luminescence.y + luminescence.z);
}


void PostProcess::HDRProcess(unsigned int capturingFramebuffer)
{
	const GraphicsSettings& param = SystemManager::GetRenderSystem().graphicsSettings;

	glBindFramebuffer(GL_FRAMEBUFFER, capturingFramebuffer);
	m_hdrShader.Bind();
	glBindTextureUnit(0, m_capturedTexture);

	float exposure = SystemManager::GetCameraSystem().renderingCamera->GetCurrentExposure();

	m_hdrShader.SendUniform("uExposure", &exposure);
	m_hdrShader.SendUniform("uBloom", &param.bloomEnabled);
	m_hdrShader.SendUniform("uBloomThreshold", &param.bloomThreshold);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	m_hdrShader.Unbind();
}


void PostProcess::GaussianBlurProcess()
{
	bool horizontal = true;

	//	Gaussian blur
	{
		bool firstIteration = true;
		size_t amount = 5;
		m_blurShader.Bind();

		for (size_t i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFrameBuffer[horizontal]);

			int h = horizontal;
			m_blurShader.SendUniform("uHorizontal", &h);

			if (firstIteration)
			{
				glBindTextureUnit(0, m_bloomTexture);
				firstIteration = false;
			}
			else glBindTextureUnit(0, m_pingpongTexture[!horizontal]);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			horizontal = !horizontal;
		}
		m_blurShader.Unbind();
	}

	m_treatedBloomTexture = m_pingpongTexture[static_cast<int>(!horizontal)];
}


void PostProcess::Process()
{
	const GraphicsSettings& param = SystemManager::GetRenderSystem().graphicsSettings;

	//m_screenRect.Bind();
	SystemManager::GetRenderSystem().utils.screenRect.Bind();
	
	//	Automatic Exposure
	if (param.autoExposure)
	{
		Time& time = *EngineContext::Instance().time;
		m_brightnessCalculationElapsedTime += time.GetUnscaledDeltaTime();

		while (m_brightnessCalculationElapsedTime > param.brightnessCalculFrequency)
		{
			ComputeBrightnessFromTexture(m_capturedTexture);
			m_brightnessCalculationElapsedTime = 0.0f;
		}
	}

	SystemManager::GetCameraSystem().renderingCamera->UpdateExposure();

	glViewport(0, 0, static_cast<GLsizei>(m_dimensions.x), static_cast<GLsizei>(m_dimensions.y));
	glDisable(GL_DEPTH_TEST);


	HDRProcess(m_bloomFrameBuffer);

	if (param.bloomEnabled)
	{
		GaussianBlurProcess();
	}

	//	Unbinds & Resets
	glBindTextureUnit(0, 0);
	glBindTextureUnit(1, 0);
	SystemManager::GetRenderSystem().utils.screenRect.Unbind();
	glEnable(GL_DEPTH_TEST);
}

void PostProcess::Draw()
{
	const GraphicsSettings& param = SystemManager::GetRenderSystem().graphicsSettings;

	glViewport(0, 0, static_cast<GLsizei>(m_dimensions.x), static_cast<GLsizei>(m_dimensions.y));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glBindTextureUnit(0, m_colorTexture);
	glBindTextureUnit(1, m_treatedBloomTexture);

	SystemManager::GetRenderSystem().utils.screenRect.Bind();
	m_finalShader.Bind();
	m_finalShader.SendUniform("uBloom", &param.bloomEnabled);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	m_finalShader.Unbind();
	SystemManager::GetRenderSystem().utils.screenRect.Unbind();

	glBindTextureUnit(0, 0);
	glBindTextureUnit(1, 0);
	glEnable(GL_DEPTH_TEST);
}

void PostProcess::SetDimensions(const Vector2& newDimensions)
{
	if (newDimensions != m_dimensions)
	{
		m_dimensions = newDimensions;

		GLsizei x = static_cast<GLsizei>(m_dimensions.x);
		GLsizei y = static_cast<GLsizei>(m_dimensions.y);

		glBindRenderbuffer(GL_RENDERBUFFER, m_captureRenderBuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, x, y);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_capturedMSTexture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB16F, x, y, GL_TRUE);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glBindTexture(GL_TEXTURE_2D, m_colorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, m_bloomTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, m_capturedTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, m_pingpongTexture[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, m_pingpongTexture[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, NULL);

		//	Avoid /0 division
		float width = Maths::Max(m_dimensions.x, 1.f);
		float height = Maths::Max(m_dimensions.x, 1.f);
		m_ratio = width / height;

		glBindTexture(GL_TEXTURE_2D, m_brightnessTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_BrightnessTextureResolution, static_cast<GLsizei>(m_BrightnessTextureResolution / m_ratio), 0, GL_RGB, GL_FLOAT, NULL);

		m_1x1BrightnessTextureMipMapLevel = Compute1x1MipMap(
			m_BrightnessTextureResolution, 
			static_cast<int>(m_BrightnessTextureResolution / m_ratio)
		);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

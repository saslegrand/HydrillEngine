#pragma once

#include "Maths/Vector2.hpp"
#include "Renderer/Primitives/VertexArray.hpp"
#include "Renderer/Primitives/ShaderProgram.hpp"

class PostProcess
{

//	Variables

private:

	ShaderProgram m_hdrShader;
	ShaderProgram m_blurShader;
	ShaderProgram m_luminanceShader;
	ShaderProgram m_finalShader;

	int m_savedFB = 0;
	unsigned int m_captureFrameBuffer;
	unsigned int m_capturedTexture;
	unsigned int m_captureRenderBuffer;
	unsigned int m_intermediateCaptureFrameBuffer;
	unsigned int m_capturedMSTexture;

	unsigned int m_brightnessFrameBuffer;
	unsigned int m_brightnessTexture;

	unsigned int m_bloomFrameBuffer;
	unsigned int m_bloomTexture;
	unsigned int m_colorTexture;

	unsigned int m_pingpongFrameBuffer[2];
	unsigned int m_pingpongTexture[2];

	unsigned int m_treatedBloomTexture;

	float m_ratio = 1920.f / 1080.f;
	Vector2 m_dimensions = Vector2(1920.f, 1080.f);

	int m_BrightnessTextureResolution = 64;
	
	unsigned int m_1x1BrightnessTextureMipMapLevel;
	float m_brightnessCalculationElapsedTime = 0.0f;

public:

	bool autoExposure = true;

//	Functions

private:

	void InitializeCaptureProcess();
	void InitializeLuminanceProcess();
	void InitializeBloomProcess();

	void ComputeBrightnessFromTexture(unsigned int texture);
	void HDRProcess(unsigned int capturingFramebuffer);
	void GaussianBlurProcess();

	unsigned int Compute1x1MipMap(int width, int length);

public:

	void Initialize();

	void BeginFrameBufferDraw();
	void EndFrameBufferDraw();

	void Process();
	void Draw();

	void SetDimensions(const Vector2& newDimensions);
};
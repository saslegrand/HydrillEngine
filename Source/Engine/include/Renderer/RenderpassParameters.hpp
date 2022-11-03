#pragma once

#include "Renderer/PostProcess.hpp"
#include "Renderer/RenderObjects/Camera.hpp"
#include "Renderer/Primitives/FrameBuffer.hpp"
#include "Maths/Vector2.hpp"
#include "EngineDLL.hpp"


class RenderpassParameters
{

//	Variables

private:

	std::unique_ptr<PostProcess> m_postProcess;

	Vector2 m_viewportPosition = Vector2(0.0f, 0.0f);
	Vector2 m_viewportDimensions  = Vector2(1920.f, 1080.f);

	Vector2 m_containerDimensions = Vector2(1920.f, 1080.f);
	FrameBuffer frameBuffer;

	float m_viewportRatio  = 1920.f / 1080.f;
	float m_containerRatio = 1920.f / 1080.f;
	
	Camera* m_camera = nullptr;

//	Functions

public:

	ENGINE_API void Initialize();
	ENGINE_API void BindCamera(Camera* camera);
	ENGINE_API Camera* GetBindedCamera();

	ENGINE_API void SetViewportDimensions(Vector2 dimensions);
	ENGINE_API void SetViewportDimensions(float x, float y);
	ENGINE_API void SetViewportRatio(float ratio);
	ENGINE_API const Vector2& GetViewportDimensions() const;
	ENGINE_API const Vector2& GetViewportPosition() const;

	ENGINE_API void SetContainerDimensions(Vector2 dimensions);
	ENGINE_API void SetContainerDimensions(float x, float y);
	ENGINE_API const Vector2& GetContainerDimensions() const;

	void BeginCapture();
	void EndCapture();

	ENGINE_API void UpdateFrameBuffer();
	
	ENGINE_API unsigned int GetRenderedImage();
	ENGINE_API void DrawImage();

	ENGINE_API void EnablePostProcess();
	ENGINE_API void DisablePostProcess();
};
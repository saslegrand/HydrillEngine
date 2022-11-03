#include "Renderer/RenderpassParameters.hpp"

#include <glad/gl.h>

#include "ECS/Systems/SystemManager.hpp"
#include "Renderer/RenderSystem.hpp"

void RenderpassParameters::Initialize()
{
	frameBuffer.Create();
}

void RenderpassParameters::BindCamera(Camera* camera)
{
	if (m_camera != camera)
	{
		m_camera = camera;
		m_camera->SetAspect(m_viewportDimensions);
	}
}

Camera* RenderpassParameters::GetBindedCamera()
{
	return m_camera;
}

void RenderpassParameters::SetViewportRatio(float ratio)
{
	m_viewportRatio = ratio;
}

void RenderpassParameters::SetViewportDimensions(Vector2 dimensions)
{
	if (m_viewportDimensions != dimensions)
	{
		m_viewportDimensions = dimensions;

		if(m_camera) m_camera->SetAspect(m_viewportDimensions);

		frameBuffer.SetDimensions(m_viewportDimensions);

		if(m_postProcess) m_postProcess->SetDimensions(m_viewportDimensions);
	}
}

void RenderpassParameters::SetViewportDimensions(float x, float y)
{
	SetViewportDimensions(Vector2(x, y));
}

const Vector2& RenderpassParameters::GetViewportDimensions() const
{
	return m_viewportDimensions;
}

const Vector2& RenderpassParameters::GetViewportPosition() const
{
	return m_viewportPosition;
}


void RenderpassParameters::SetContainerDimensions(Vector2 dimensions)
{
	if (m_containerDimensions != dimensions)
	{
		m_containerDimensions = dimensions;

		Vector2 vdim = m_viewportDimensions;

		vdim.x = dimensions.y * m_viewportRatio;
		vdim.y = dimensions.y;

		if (vdim.x > dimensions.x)
		{
			vdim.x = dimensions.x;
			vdim.y = vdim.x / m_viewportRatio;
		}

		Vector2 delta = m_containerDimensions - vdim;
		m_viewportPosition.x = delta.x * 0.5f;
		m_viewportPosition.y = delta.y * 0.5f;

		SetViewportDimensions(vdim);
	}
}

void RenderpassParameters::SetContainerDimensions(float x, float y)
{
	SetContainerDimensions(Vector2(x, y));
}

const Vector2& RenderpassParameters::GetContainerDimensions() const
{
	return m_containerDimensions;
}


void RenderpassParameters::EnablePostProcess()
{
	m_postProcess = std::make_unique<PostProcess>();
	m_postProcess->Initialize();
	m_postProcess->SetDimensions(m_viewportDimensions);
}


void RenderpassParameters::DisablePostProcess()
{
	m_postProcess.reset();
}

void RenderpassParameters::BeginCapture()
{
	if (m_postProcess)
	{
		m_postProcess->BeginFrameBufferDraw();
	}
	else
	{
		frameBuffer.Bind();
		glViewport(0, 0, static_cast<GLsizei>(m_viewportDimensions.x), static_cast<GLsizei>(m_viewportDimensions.y));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

}

void RenderpassParameters::EndCapture()
{
	if (m_postProcess)
	{
		m_postProcess->EndFrameBufferDraw();
		m_postProcess->Process();
		
		frameBuffer.Bind();
		m_postProcess->Draw();
		frameBuffer.EndDraw();
		frameBuffer.Unbind();
	}
	else
	{
		frameBuffer.EndDraw();
		frameBuffer.Unbind();
	}
}

unsigned int RenderpassParameters::GetRenderedImage()
{
	return frameBuffer.GetImage();
}

void RenderpassParameters::UpdateFrameBuffer()
{
	frameBuffer.Update();
}


void RenderpassParameters::DrawImage()
{
	glViewport(static_cast<GLint>(m_viewportPosition.x), static_cast<GLint>(m_viewportPosition.y), static_cast<GLsizei>(m_viewportDimensions.x), static_cast<GLsizei>(m_viewportDimensions.y));
	SystemManager::GetRenderSystem().utils.DrawImageScreen(frameBuffer.GetImage());
}
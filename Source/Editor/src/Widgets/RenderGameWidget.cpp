#include "Widgets/RenderGameWidget.hpp"

#include <imgui/imgui.h>
#include <EngineContext.hpp>
#include <Renderer/RenderSystem.hpp>
#include <ECS/Systems/CameraSystem.hpp>
#include <ECS/Systems/SystemManager.hpp>
#include <ECS/CameraComponent.hpp>

#include "EditorTheme.hpp"
#include "GUI.hpp"

RenderGameWidget::RenderGameWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive)
	: RenderWidget(widgetName, widgetID, isActive, "###GAME")
{
	m_ratio = m_width / m_height;

	m_gameRenderParameters.Initialize();
	m_gameRenderParameters.SetContainerDimensions(1920.f, 1080.f);
	m_gameRenderParameters.EnablePostProcess();
}


void RenderGameWidget::Update()
{
	m_firstFrameFocus = false;

	if (!isActive) return;

	RenderWidget::Update();

	m_gameRenderParameters.UpdateFrameBuffer();

	if(isVisible) RenderFramebuffer();
}


void RenderGameWidget::Display()
{
	if (!isActive) return;

	if(BeginWidget())
	{
		BeginRenderChildWindow();

		ImVec2 dimension = ImGui::GetWindowSize();
		
		m_gameRenderParameters.SetViewportRatio(m_ratio);
		m_gameRenderParameters.SetContainerDimensions(dimension.x, dimension.y);

		ImVec2 pos = {
			m_gameRenderParameters.GetViewportPosition().x,
			m_gameRenderParameters.GetViewportPosition().y
		};
		ImVec2 dim = {
			m_gameRenderParameters.GetViewportDimensions().x,
			m_gameRenderParameters.GetViewportDimensions().y
		};
		
		ImGui::SetCursorPos(pos);
		ImGui::Image(GUI::CastTextureID(m_gameRenderParameters.GetRenderedImage()), dim, ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
		EndRenderChildWindow();
	}
	EndWidget();
}


void RenderGameWidget::RenderFramebuffer()
{
	CameraComponent* camComp = CameraSystem::GetActiveCameraComponent();

	if (camComp == nullptr) return;

	m_gameRenderParameters.BindCamera(&camComp->camera);
	SystemManager::GetRenderSystem().Render(m_gameRenderParameters);
}

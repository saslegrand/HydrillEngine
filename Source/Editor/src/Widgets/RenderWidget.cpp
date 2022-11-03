#include "Widgets/RenderWidget.hpp"

#include <imgui/imgui.h>
#include <glad/gl.h>

#include <Core/Logger.hpp>
#include <IO/Mouse.hpp>
#include <IO/Keyboard.hpp>

#include "EditorContext.hpp"


RenderWidget::RenderWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive, const char* typeID)
	: EditorWidget(widgetName, widgetID, isActive, typeID)
{
	m_flag |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
}


void RenderWidget::Update()
{
	if (m_captured)
	{
		if (Input::Keyboard::GetKeyPressed(Key::Escape) || !isActive)
		{
			Input::Mouse::SetMode(MouseMode::Normal);

			m_captured = false;
		}
	}
}


bool RenderWidget::BeginRenderChildWindow()
{
	bool opened = ImGui::BeginChild("RenderPlace", ImGui::GetContentRegionAvail());

	if (opened && ImGui::IsWindowHovered())
	{
		if (Input::Mouse::GetButtonPressed(MouseButton::Right))
		{
			ImGui::SetWindowFocus();
			Input::Mouse::SetMode(MouseMode::Disabled);
			Input::Mouse::ResetDeltaPosition();

			m_firstFrameFocus = true;
			m_captured = true;
		}
	}

	return opened;
}


void RenderWidget::EndRenderChildWindow()
{
	ImGui::EndChild();
}


bool RenderWidget::BeginWidget()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1, 1));
	isVisible = EditorWidget::BeginWidget();
	ImGui::PopStyleVar(1);

	if (isVisible)
	{
		ParameterBar();	
	}

	return isVisible;
}
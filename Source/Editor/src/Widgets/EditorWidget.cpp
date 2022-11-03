#include "Widgets/EditorWidget.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <IO/Mouse.hpp>
#include <Core/Logger.hpp>

#include "EditorContext.hpp"


EditorWidget::EditorWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive, const std::string& typeID)
	: isActive(isActive), m_ID(typeID + std::to_string(widgetID))
{
	m_flag |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
	m_windowClass = std::make_unique<ImGuiWindowClass>();
	m_windowClass->DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoCloseButton;
	Rename(widgetName);
}


EditorWidget::~EditorWidget() 
{
}


void EditorWidget::Update()
{
}


void EditorWidget::Display()
{
	if (!isActive) return;

	BeginWidget();

	ImGui::Text(("This is " + std::string(m_name) + " window.").c_str());

	EndWidget();
}


bool EditorWidget::BeginWidget()
{
	ImGui::SetNextWindowClass(m_windowClass.get());
	isVisible = ImGui::Begin(m_nameID.c_str(), &isActive, m_flag);

	const ImGuiViewport* usedViewport = ImGui::GetWindowViewport();
	const ImGuiViewport* mainViewport = ImGui::GetMainViewport();

	if (!m_externViewport && usedViewport  != mainViewport)
	{
		m_flag |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
		m_externViewport = true;
	}
	else if (m_externViewport && usedViewport == mainViewport)
	{
		m_flag &= ~ImGuiWindowFlags_NoDecoration & ~ImGuiWindowFlags_NoMove;
		m_externViewport = false;
	}

	if (isVisible && m_keepFocus) ImGui::SetWindowFocus();
	
	m_keepFocus = false;

	return isVisible;
}


void EditorWidget::EndWidget()
{
	ImGui::End();
}


void EditorWidget::Rename(const std::string& newName)
{
	m_name = newName;

	if (m_name.size() > WIDGETNAME_CHAR_MAX)
	{
		m_name.resize(WIDGETNAME_CHAR_MAX);
		Logger::Warning("Widget name is too long ! The name will be forcefully shortened (" + m_name + ") Maximum character for a widget name is " + std::to_string(WIDGETNAME_CHAR_MAX));
	}

	m_nameID = m_name + m_ID;
}


std::string EditorWidget::GetName() const
{
	return m_name;
}
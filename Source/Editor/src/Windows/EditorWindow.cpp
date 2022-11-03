#include "Windows/EditorWindow.hpp"

#include <imgui/imgui.h>

#include <IO/Mouse.hpp>
#include <IO/Window.hpp>
#include <Core/Logger.hpp>

#include "EditorContext.hpp"
#include "GUI.hpp"


EditorWindow::EditorWindow(const std::string& widgetName,  const bool isActive, const std::string& typeID)
	: isActive(isActive), m_ID(typeID)
{
	m_flag |= ImGuiWindowFlags_NoCollapse;
	m_windowClass = std::make_unique<ImGuiWindowClass>();
	
	Rename(widgetName);
}


EditorWindow::~EditorWindow()
{
}


void EditorWindow::Update()
{
}


void EditorWindow::Display()
{
	if (BeginWindow())
	{


		EndWindow();
	}
}


bool EditorWindow::BeginWindow()
{
	ImGui::SetNextWindowClass(m_windowClass.get());

	GUI::PushColor(ImGuiCol_TitleBg, EditorContext::Instance().theme->color_buttons.hoveredColor);
	GUI::PushColor(ImGuiCol_TitleBgActive, EditorContext::Instance().theme->color_buttons.hoveredColor);
	GUI::PushColor(ImGuiCol_WindowBg, EditorContext::Instance().theme->color_buttons.hoveredColor);
	
	bool out = ImGui::Begin(m_nameID.c_str(), &isActive, m_flag);

	if (initializeSize)
	{
		ImGui::SetWindowSize(ImVec2(EditorContext::Instance().window->GetWidth() * 0.6f, EditorContext::Instance().window->GetHeight() * 0.6f));
		initializeSize = false;
	}
	GUI::PopColor(3);

	const ImGuiViewport* usedViewport = ImGui::GetWindowViewport();
	const ImGuiViewport* mainViewport = ImGui::GetMainViewport();

	if (!m_externViewport && usedViewport != mainViewport)
	{
		m_flag |= ImGuiWindowFlags_NoDecoration;
		m_externViewport = true;
	}
	else if (m_externViewport && usedViewport == mainViewport)
	{
		m_flag -= m_flag & ImGuiWindowFlags_NoDecoration;
		m_externViewport = false;
	}

	return out;
}


void EditorWindow::EndWindow()
{
	ImGui::End();
}


void EditorWindow::Rename(const std::string& newName)
{
	m_name = newName;

	if (m_name.size() > WINDOWNAME_CHAR_MAX)
	{
		m_name.resize(WINDOWNAME_CHAR_MAX);
		Logger::Warning("Window name is too long ! The name will be forcefully shortened (" + m_name + ") Maximum character for a window name is " + std::to_string(WINDOWNAME_CHAR_MAX));
	}

	m_nameID = m_name + m_ID;
}


std::string EditorWindow::GetName() const
{
	return m_name;
}
#include "GUITypes.hpp"

#include <imgui/imgui.h>

#include "GUI.hpp"
#include "EditorContext.hpp"
#include "PathConfig.hpp"

GUI::FontGroup::FontGroup(const float size, ImGuiIO* io)
{
	m_io = io;
	m_size = size;
}

void GUI::FontGroup::LoadFont(ImFont*& out_font, const char* path, const bool isDefault)
{
	std::string fullPath = INTERNAL_EDITOR_RESOURCES_ROOT + std::string(path);

	if (m_io)
	{
		out_font = m_io->Fonts->AddFontFromFileTTF(fullPath.c_str(), m_size);

		if (isDefault)
		{
			m_io->FontDefault = out_font;
		}
	}
}

void GUI::FontGroup::AddFont(const unsigned int fontType, const char* path, const bool isdefault)
{
	switch (fontType)
	{
	case FontTypes_Regular:	LoadFont(m_regular, path, isdefault); break;
	case FontTypes_Light:	LoadFont(m_light, path, isdefault);	  break;
	case FontTypes_Bold:	LoadFont(m_bold, path, isdefault);	  break;
	case FontTypes_Italic:	LoadFont(m_italic, path, isdefault);  break;
	case FontTypes_Thin:	LoadFont(m_thin, path, isdefault);	  break;
	}
}

void GUI::FontGroup::UseRegular()
{
	ImGui::PushFont(m_regular);
} 

void GUI::FontGroup::UseLight()
{
	ImGui::PushFont(m_light);
}

void GUI::FontGroup::UseBold()
{
	ImGui::PushFont(m_bold);
}

void GUI::FontGroup::UseItalic()
{
	ImGui::PushFont(m_italic);
}

void GUI::FontGroup::UseThin()
{
	ImGui::PushFont(m_thin);
}

void GUI::FontGroup::EndFontStyle()
{
	ImGui::PopFont();
}




void GUI::ColorTheme::AutoGen()
{
	if (config == COLORTHEME_LIGHT)
	{
		hoveredColor = defaultColor * 1.10f;
		activeColor = defaultColor * 1.20f;
		unfocusedInactiveColor = defaultColor;
		unfocusedActiveColor = defaultColor * 1.20f;
	}
	else if (config == COLORTHEME_DARK)
	{
		hoveredColor = defaultColor * 0.90f;
		activeColor = defaultColor * 0.80f;
		unfocusedInactiveColor = defaultColor;
		unfocusedActiveColor = defaultColor * 0.80f;
	}
}



GUI::Dockspace::Dockspace(const char* name, const char* dockID)
{
	m_name = name;
	m_dockID = dockID;

	ImGuiIO& io = ImGui::GetIO();

	//  Force enable docking
	if ((io.ConfigFlags & ImGuiConfigFlags_DockingEnable) == false)
	{
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	//  Generate flags

	m_dockFlags = ImGuiDockNodeFlags_None;

	m_windowFlags =
		ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus;


	if (m_dockFlags & ImGuiDockNodeFlags_PassthruCentralNode)
	{
		m_windowFlags |= ImGuiWindowFlags_NoBackground;
	}

	m_initialized = true;
}


bool GUI::Dockspace::Begin()
{
	if (m_initialized == false) return false;

	//	Forcefully set window transform
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
	}


	//	Set special styles for thiswindow
	{
		GUI::PushStyle(ImGuiStyleVar_WindowRounding, 0.0f);
		GUI::PushStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
		GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		GUI::PushStyle(ImGuiStyleVar_ChildRounding, 0.5f);
		GUI::PushStyle(ImGuiStyleVar_ChildBorderSize, 0.5f);

		GUI::PushColor(ImGuiCol_WindowBg, Color4(0.0f, 0.0f, 0.0f, 1.0f));
		GUI::PushColor(ImGuiCol_DockingEmptyBg, Color4(0.0f, 0.0f, 0.0f, 1.0f));

		// When using ImGuiDockNodeFlags_PassthruCentralNode, Dockspace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		ImGui::Begin(m_name, nullptr, m_windowFlags);

		GUI::PopColor(2);

		GUI::PopStyle(5);
	}

	// Submit the Dockspace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID(m_dockID);
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_dockFlags);

		return true;
	}

	return false;
}



void GUI::Dockspace::End()
{
	if (m_initialized == false) return;
	ImGui::End();
}
#include "EditorTheme.hpp"

#include <string>
#include <cmath>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <Types/Color.hpp>

#include "GUI.hpp"
#include "EditorContext.hpp"
#include "EditorData.hpp"

#define SPACING 10.f
#define PADDDING 50.f

EditorTheme::EditorTheme()
{
	IO = &ImGui::GetIO();

	style = &ImGui::GetStyle();
	m_styleColors = style->Colors;

	style->FramePadding  = ImVec2(5.f, 5.f);
	style->WindowPadding = ImVec2(15.f, 15.f);

	style->ItemSpacing      = ImVec2(5.f, 5.f);
	style->ItemInnerSpacing = ImVec2(5.f, 5.f);
	style->WindowMenuButtonPosition = ImGuiDir_None;

	style->IndentSpacing  = 25.f;
	style->WindowRounding = 0.0f;
	style->ChildRounding  = 2.5f;
	style->PopupRounding  = 2.5f;
	style->TabRounding    = 2.5f;
	style->FrameRounding  = 7.5f;
	style->GrabRounding   = 7.5f;

	style->FrameBorderSize = 0.0f;
	style->ChildBorderSize = 0.0f;
	style->TabBorderSize   = 0.0f;
	style->PopupBorderSize = 0.0f;

	style->TabMinWidthForCloseButton = 50.f;
	style->DisabledAlpha = 0.25f;

	color_backgrounds.defaultColor = { 0.13f, 0.13f, 0.16f, 1.f };
	color_borders.defaultColor = { 0.09f, 0.09f, 0.11f, 1.f };
	color_buttons.defaultColor = { 0.15f, 0.15f, 0.18f, 1.f };
	color_interactives.defaultColor = { 0.2f, 0.3f, 0.4f, 0.98f };
	color_interactiveBackgrounds.defaultColor = { 0.08f, 0.08f, 0.10f, 1.f };

	color_backgrounds.config = GUI::COLORTHEME_LIGHT;
	color_borders.config = GUI::COLORTHEME_LIGHT;
	color_buttons.config = GUI::COLORTHEME_LIGHT;
	color_interactives.config = GUI::COLORTHEME_DARK;
	color_interactiveBackgrounds.config = GUI::COLORTHEME_LIGHT;
	//	Load Default Fonts

	defaultFont = std::make_unique<GUI::FontGroup>(15.f, IO);

	defaultFont->AddFont(FontTypes_Regular, R"(Fonts\Roboto\Roboto-Regular.ttf)", true);
	defaultFont->AddFont(FontTypes_Light,   R"(Fonts\Roboto\Roboto-Light.ttf)");
	defaultFont->AddFont(FontTypes_Bold,    R"(Fonts\Roboto\Roboto-Bold.ttf)");
	defaultFont->AddFont(FontTypes_Italic,  R"(Fonts\Roboto\Roboto-Italic.ttf)");
	defaultFont->AddFont(FontTypes_Thin,    R"(Fonts\Roboto\Roboto-Thin.ttf)");

	consoleFont = std::make_unique<GUI::FontGroup>(15.f, IO);
	consoleFont->AddFont(FontTypes_Regular, R"(Fonts\RobotoMono\RobotoMono-Regular.ttf)");
	consoleFont->AddFont(FontTypes_Light, R"(Fonts\RobotoMono\RobotoMono-Light.ttf)");
	consoleFont->AddFont(FontTypes_Bold, R"(Fonts\RobotoMono\RobotoMono-Bold.ttf)");
	consoleFont->AddFont(FontTypes_Italic, R"(Fonts\RobotoMono\RobotoMono-Italic.ttf)");
	consoleFont->AddFont(FontTypes_Thin,    R"(Fonts\RobotoMono\RobotoMono-Thin.ttf)");

	m_colorX = { 0.5f, 0.2f, 0.3f, 1.0f };
	m_colorY = { 0.2f, 0.5f, 0.3f, 1.0f };
	m_colorZ = { 0.2f, 0.3f, 0.5f, 1.0f };
	m_colorW = { 0.7f, 0.5f, 0.1f, 1.0f };
}


EditorTheme::~EditorTheme()
{
}


//	Theme

void EditorTheme::SetStyleColor(const int target, const Color4& color)
{
	m_styleColors[target] = ImVec4(color.r, color.g, color.b, color.a);
}


void EditorTheme::InitializeColors()
{
	EditorData* data = EditorContext::Instance().editorDatas;

	color_backgrounds.AutoGen();
	color_borders.AutoGen();
	color_buttons.AutoGen();
	color_interactives.AutoGen();
	color_interactiveBackgrounds.AutoGen();

	police = { 0.95f, 0.95f, 0.95f, 1.f };
	disabledPolice = { 0.35f, 0.35f, 0.35f, 1.f };
}



void EditorTheme::SetColors()
{
	//  Polices
	SetStyleColor(ImGuiCol_Text, police);
	SetStyleColor(ImGuiCol_TextDisabled, disabledPolice);

	//  Backgrounds
	SetStyleColor(ImGuiCol_WindowBg, color_backgrounds.defaultColor);
	SetStyleColor(ImGuiCol_ChildBg, color_backgrounds.defaultColor);
	SetStyleColor(ImGuiCol_PopupBg, color_backgrounds.activeColor);

	//  Borders
	SetStyleColor(ImGuiCol_Border, color_borders.defaultColor);
	SetStyleColor(ImGuiCol_BorderShadow, color_borders.defaultColor);

	//  Frames
	SetStyleColor(ImGuiCol_FrameBg, color_interactiveBackgrounds.hoveredColor);
	SetStyleColor(ImGuiCol_FrameBgHovered, color_interactiveBackgrounds.activeColor);
	SetStyleColor(ImGuiCol_FrameBgActive, color_interactiveBackgrounds.activeColor);

	//  Titles
	SetStyleColor(ImGuiCol_TitleBg, color_borders.hoveredColor);
	SetStyleColor(ImGuiCol_TitleBgActive, color_borders.hoveredColor);
	SetStyleColor(ImGuiCol_TitleBgCollapsed, color_borders.hoveredColor);

	//  Menu Bars
	SetStyleColor(ImGuiCol_MenuBarBg, color_backgrounds.defaultColor);
	
	//  Scrollbars
	SetStyleColor(ImGuiCol_ScrollbarBg, color_backgrounds.defaultColor);
	SetStyleColor(ImGuiCol_ScrollbarGrab, color_interactiveBackgrounds.defaultColor);
	SetStyleColor(ImGuiCol_ScrollbarGrabHovered, color_interactiveBackgrounds.hoveredColor);
	SetStyleColor(ImGuiCol_ScrollbarGrabActive, color_interactiveBackgrounds.activeColor);

	//  Check Marks
	SetStyleColor(ImGuiCol_CheckMark, color_interactives.defaultColor);

	//  Sliders
	SetStyleColor(ImGuiCol_SliderGrab, color_interactives.defaultColor);
	SetStyleColor(ImGuiCol_SliderGrabActive, color_interactives.activeColor);

	//  Buttons
	SetStyleColor(ImGuiCol_Button, color_buttons.defaultColor);
	SetStyleColor(ImGuiCol_ButtonHovered, color_buttons.hoveredColor);
	SetStyleColor(ImGuiCol_ButtonActive, color_buttons.activeColor);

	//  Headers
	SetStyleColor(ImGuiCol_Header, color_interactives.defaultColor);
	SetStyleColor(ImGuiCol_HeaderHovered, color_interactives.hoveredColor);
	SetStyleColor(ImGuiCol_HeaderActive, color_interactives.activeColor);

	//  Separators
	SetStyleColor(ImGuiCol_Separator, color_borders.hoveredColor);
	SetStyleColor(ImGuiCol_SeparatorHovered, color_borders.hoveredColor);
	SetStyleColor(ImGuiCol_SeparatorActive, color_borders.hoveredColor);

	//  Resize Grips
	SetStyleColor(ImGuiCol_ResizeGrip, color_backgrounds.defaultColor);
	SetStyleColor(ImGuiCol_ResizeGripHovered, color_backgrounds.hoveredColor);
	SetStyleColor(ImGuiCol_ResizeGripActive, color_backgrounds.activeColor);

	//  Tabs
	SetStyleColor(ImGuiCol_Tab, color_buttons.defaultColor);
	SetStyleColor(ImGuiCol_TabHovered, color_buttons.hoveredColor);
	SetStyleColor(ImGuiCol_TabActive, color_buttons.activeColor);
	SetStyleColor(ImGuiCol_TabUnfocused, color_buttons.unfocusedInactiveColor);
	SetStyleColor(ImGuiCol_TabUnfocusedActive, color_buttons.unfocusedActiveColor);

	//  Docking Previews
	SetStyleColor(ImGuiCol_DockingPreview, color_interactives.activeColor);
	SetStyleColor(ImGuiCol_DockingEmptyBg, color_backgrounds.defaultColor);

	//Plot Lines
	SetStyleColor(ImGuiCol_PlotLines, color_interactives.defaultColor);
	SetStyleColor(ImGuiCol_PlotLinesHovered, color_interactives.hoveredColor);
	SetStyleColor(ImGuiCol_PlotHistogram, color_interactives.defaultColor);
	SetStyleColor(ImGuiCol_PlotHistogramHovered, color_interactives.hoveredColor);

	SetStyleColor(ImGuiCol_TableHeaderBg, color_backgrounds.defaultColor);
	SetStyleColor(ImGuiCol_TableBorderStrong, color_borders.defaultColor);
	SetStyleColor(ImGuiCol_TableBorderLight, color_borders.defaultColor);
	SetStyleColor(ImGuiCol_TableRowBg, color_backgrounds.defaultColor);
	SetStyleColor(ImGuiCol_TableRowBgAlt, color_backgrounds.activeColor);

	//  Selected Texts
	SetStyleColor(ImGuiCol_TextSelectedBg, color_interactives.defaultColor);

	//  Drag and Drop Targets
	SetStyleColor(ImGuiCol_DragDropTarget, color_borders.activeColor);

	//  Nav
	SetStyleColor(ImGuiCol_NavHighlight, { 0.10f, 0.10f, 0.14f, 0.70f });
	SetStyleColor(ImGuiCol_NavWindowingHighlight, { 0.10f, 0.10f, 0.14f, 0.70f });
	SetStyleColor(ImGuiCol_NavWindowingDimBg, { 0.10f, 0.10f, 0.14f, 0.70f });
	SetStyleColor(ImGuiCol_ModalWindowDimBg, { 0.10f, 0.10f, 0.14f, 0.70f });
}
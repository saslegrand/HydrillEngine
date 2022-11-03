#include "Windows/PreferencesWindow.hpp"

#include <filesystem>
#include <imgui/imgui.h>
#include <nlohmann/json.hpp>

#include <IO/Mouse.hpp>
#include <Core/Logger.hpp>
#include <Core/TaskQueue.hpp>
#include <Tools/SerializationUtils.hpp>

#include "GUI.hpp"
#include "EditorContext.hpp"
#include "EditorData.hpp"
#include "PathConfig.hpp"

constexpr char Preferences_Save_File[] = R"(Saves\\Preferences.settings)";

PreferencesWindow::PreferencesWindow(const std::string& widgetName, const bool isActive)
	: EditorWindow(widgetName, isActive, "###PREFERENCE")
{
	m_flag |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	m_windowClass = std::make_unique<ImGuiWindowClass>();
	
	PreferenceCategory& contentBrowser = m_categories.emplace_back();
	contentBrowser.name = "Content Browser";
	contentBrowser.displayFunction = std::bind(&PreferencesWindow::DisplayContentBrowserPreferences, this);

	PreferenceCategory& themeSetting = m_categories.emplace_back();
	themeSetting.name = "Theme";
	themeSetting.displayFunction = std::bind(&PreferencesWindow::DisplayThemePreferences, this);

	m_selectedCategory = &m_categories[0];

	prefSavePath = INTERNAL_EDITOR_RESOURCES_ROOT + std::string(Preferences_Save_File);
}


PreferencesWindow::~PreferencesWindow()
{
}


void PreferencesWindow::Update()
{
}


void PreferencesWindow::Display()
{
	if (!isActive) return;

	GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
	bool opened = BeginWindow();
	GUI::PopStyle();

	if(opened)
	{
		float width = ImGui::GetContentRegionAvail().x;

		if (ImGui::BeginTable("###PREFERENCES_TABLE", 2, ImGuiTableFlags_SizingFixedFit))
		{
			ImGui::TableSetupColumn(ImGui::TableGetColumnName(0), 0, width * 0.25f);
			ImGui::TableSetupColumn(ImGui::TableGetColumnName(1), 0, width * 0.75f);

			ImGui::TableNextColumn();

			GUI::PushColor(ImGuiCol_ChildBg, EditorContext::Instance().theme->color_backgrounds.defaultColor);
			bool categoriesOpenend = ImGui::BeginChild("CATEGORY SELECTOR", ImGui::GetContentRegionAvail(), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
			GUI::PopColor();

			if (categoriesOpenend)
			{
				for (PreferenceCategory& s : m_categories)
				{
					bool selected = &s == m_selectedCategory;

					if (ImGui::Selectable(s.name.c_str(), &selected))
					{
						m_selectedCategory = &s;
					}
				}

			}
			ImGui::EndChild();

			ImGui::TableNextColumn();

			GUI::PushColor(ImGuiCol_ChildBg, EditorContext::Instance().theme->color_backgrounds.defaultColor);
			bool activecategoryOpened = ImGui::BeginChild("SELECTED CATEGORY", ImGui::GetContentRegionAvail(), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
			GUI::PopColor();

			if (activecategoryOpened)
			{

				EditorContext::Instance().theme->defaultFont->UseBold();

				float titleWidth = ImGui::GetContentRegionAvail().x;
				float titleSize = ImGui::CalcTextSize(m_selectedCategory->name.c_str()).x;
				ImGui::SetCursorPosX(titleWidth * 0.5f - titleSize * 0.5f);
				ImGui::Text(m_selectedCategory->name.c_str());

				EditorContext::Instance().theme->defaultFont->EndFontStyle();

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				m_selectedCategory->displayFunction();
			}
			ImGui::EndChild();

			ImGui::EndTable();
		}
	}
	EndWindow();
}

bool DisplayColorPicker(const char* ID, GUI::ColorTheme& colorTarget)
{
	ImGui::PushID(ID);
	Color4 col = colorTarget.defaultColor;
	static int mode = GUI::COLORTHEME_LIGHT;

	bool modified = false;
	
	if (ImGui::ColorEdit4("###Color", &col.r))
	{
		colorTarget.defaultColor = col;
		modified = true;
	}

	ImGui::SameLine();

	bool light = colorTarget.config == GUI::COLORTHEME_LIGHT;
	if (ImGui::RadioButton("Lighter", light))
	{
		colorTarget.config = GUI::COLORTHEME_LIGHT;
		modified = true;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("Darker", !light))
	{
		colorTarget.config = GUI::COLORTHEME_DARK;
		modified = true;
	}
	ImGui::PopID();

	return modified;
}

void PreferencesWindow::DisplayThemePreferences()
{
	EditorData* datas = EditorContext::Instance().editorDatas;
	EditorTheme* theme = EditorContext::Instance().theme;

	float panelWidth = ImGui::GetContentRegionAvail().x;

	const Color4& bg = theme->color_backgrounds.defaultColor;
	Color4 borderColor = Color4::LightGray;
	if((bg.r+ bg.g + bg.b) / 3.f > 0.5f) borderColor = Color4::Black;
	borderColor.a = 0.5f;

	GUI::PushStyle(ImGuiStyleVar_FrameBorderSize, 1.f);
	GUI::PushColor(ImGuiCol_Border, borderColor);
	if (ImGui::BeginTable("", 2, ImGuiTableFlags_SizingStretchProp))
	{
		ImGui::TableSetupColumn(ImGui::TableGetColumnName(0), ImGuiTableColumnFlags_NoResize, panelWidth * 0.25f);
		ImGui::TableSetupColumn(ImGui::TableGetColumnName(1), ImGuiTableColumnFlags_NoResize, panelWidth * 0.75f);

		ImGui::TableNextColumn();

		bool modified = false;

		ImGui::Text("Background Color");
		ImGui::TableNextColumn();
		modified |= DisplayColorPicker("###BGColor", theme->color_backgrounds);
		ImGui::TableNextColumn();

		ImGui::Text("Borders Color");
		ImGui::TableNextColumn();
		modified |= DisplayColorPicker("###BRDColor", theme->color_borders);
		ImGui::TableNextColumn();

		ImGui::Text("Buttons Color");
		ImGui::TableNextColumn();
		modified |= DisplayColorPicker("###BTNColor", theme->color_buttons);
		ImGui::TableNextColumn();

		ImGui::Text("Interactives Color");
		ImGui::TableNextColumn();
		modified |= DisplayColorPicker("###INTColor", theme->color_interactives);
		ImGui::TableNextColumn();

		ImGui::Text("Interactives Background Color");
		ImGui::TableNextColumn();
		modified |= DisplayColorPicker("###INTBGColor", theme->color_interactiveBackgrounds);
		ImGui::TableNextColumn();


		if (ImGui::Button("Reset"))
		{
			theme->color_backgrounds.defaultColor = datas->themeData.backgrounds;
			theme->color_borders.defaultColor = datas->themeData.borders;
			theme->color_buttons.defaultColor = datas->themeData.buttons;
			theme->color_interactives.defaultColor = datas->themeData.interactiveColor;
			theme->color_interactiveBackgrounds.defaultColor = datas->themeData.interactiveBackgroundColor;
			modified |= true;
		}

		if (modified)
		{
			EditorContext::Instance().taskQueue->AddTask([theme]()
				{
					theme->InitializeColors();
					theme->SetColors();
				});
		}

		ImGui::SameLine();

		if (ImGui::Button("Save"))
		{
			datas->themeData.backgrounds				= theme->color_backgrounds.defaultColor;
			datas->themeData.borders					= theme->color_borders.defaultColor;
			datas->themeData.buttons					= theme->color_buttons.defaultColor;
			datas->themeData.interactiveColor			= theme->color_interactives.defaultColor;
			datas->themeData.interactiveBackgroundColor = theme->color_interactiveBackgrounds.defaultColor;
		}

		ImGui::EndTable();
	}
	GUI::PopColor();
	GUI::PopStyle();
}


void PreferencesWindow::DisplayContentBrowserPreferences()
{
	EditorData* datas = EditorContext::Instance().editorDatas;

	float panelWidth = ImGui::GetContentRegionAvail().x;

	if (ImGui::BeginTable("", 2, ImGuiTableFlags_SizingStretchProp))
	{
		ImGui::TableSetupColumn(ImGui::TableGetColumnName(0), ImGuiTableColumnFlags_NoResize, panelWidth * 0.25f);
		ImGui::TableSetupColumn(ImGui::TableGetColumnName(1), ImGuiTableColumnFlags_NoResize, panelWidth * 0.75f);

		ImGui::TableNextColumn();

		ImGui::Text("Thumbnail padding");
		ImGui::TableNextColumn();

		ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp;

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::DragFloat("##ThumbnailPadding", &datas->contentBrowser.filePadding, 0.5f, 5.f, 100.f, "%.3f", flags);
		ImGui::TableNextColumn();

		ImGui::Text("Thumbnail size");
		ImGui::TableNextColumn();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::DragFloat("###ThumbnailSize", &datas->contentBrowser.fileThumbnailSize, 0.5f, 5.f, 512.f, "%.3f", flags);

		ImGui::EndTable();
	}
}

void PreferencesWindow::Serialize()
{
	json j;

	EditorData* data = EditorContext::Instance().editorDatas;

	json& jsonContentBrowser = j["ContentBrowser"];
	jsonContentBrowser["Padding"]	= data->contentBrowser.filePadding;
	jsonContentBrowser["Thumbnail"] = data->contentBrowser.fileThumbnailSize;


	json& jsonTheme = j["Theme"];
	Serialization::SetContainer<float>(jsonTheme["background"], data->themeData.backgrounds);
	Serialization::SetContainer<float>(jsonTheme["borders"], data->themeData.borders);
	Serialization::SetContainer<float>(jsonTheme["buttons"], data->themeData.buttons);
	Serialization::SetContainer<float>(jsonTheme["interactiveColor"], data->themeData.interactiveColor);
	Serialization::SetContainer<float>(jsonTheme["interactiveBackgroundColor"], data->themeData.interactiveBackgroundColor);

	WriteJson(prefSavePath, j);
}

void PreferencesWindow::Deserialize()
{
	if (!std::filesystem::exists(prefSavePath))
		return;

	json j;
	ReadJson(prefSavePath, j);

	EditorData* data = EditorContext::Instance().editorDatas;

	json& jsonContentBrowser = j["ContentBrowser"];
	Serialization::TryGetValue(jsonContentBrowser, "Padding", data->contentBrowser.filePadding);
	Serialization::TryGetValue(jsonContentBrowser, "Thumbnail", data->contentBrowser.fileThumbnailSize);

	json& jsonTheme = j["Theme"];
	Serialization::TryGetContainer<float>(jsonTheme, "background", data->themeData.backgrounds);
	Serialization::TryGetContainer<float>(jsonTheme, "borders", data->themeData.borders);
	Serialization::TryGetContainer<float>(jsonTheme, "buttons", data->themeData.buttons);
	Serialization::TryGetContainer<float>(jsonTheme, "interactiveColor", data->themeData.interactiveColor);
	Serialization::TryGetContainer<float>(jsonTheme, "interactiveBackgroundColor", data->themeData.interactiveBackgroundColor);

	EditorTheme* theme = EditorContext::Instance().theme;
	theme->color_backgrounds.defaultColor = data->themeData.backgrounds;
	theme->color_borders.defaultColor = data->themeData.borders;
	theme->color_buttons.defaultColor = data->themeData.buttons;
	theme->color_interactives.defaultColor = data->themeData.interactiveColor;
	theme->color_interactiveBackgrounds.defaultColor = data->themeData.interactiveBackgroundColor;

	theme->InitializeColors();
	theme->SetColors();
}
#include "Windows/BuildSettingsWindow.hpp"

#include <imgui/imgui.h>
#include <nlohmann/json.hpp>

#include <Resources/Resource/Scene.hpp>
#include <Resources/ResourcesManager.hpp>
#include <Tools/SerializationUtils.hpp>
#include <Tools/FileDialogs.hpp>
#include <Renderer/RenderSystem.hpp>
#include <GameContext.hpp>
#include <Core/Logger.hpp>
#include <IO/Keyboard.hpp>
#include <IO/Mouse.hpp>

#include "Tools/GameBuilder.hpp"
#include "Tools/StringHelper.hpp"
#include "EditorContext.hpp"
#include "GUI.hpp"

constexpr char Build_Save_File[] = R"(Saves\\BuildSettings.settings)";

BuildSettingsWindow::BuildSettingsWindow(const std::string& widgetName, const bool isActive)
	: EditorWindow(widgetName, isActive, "###BUILD_SETTING")
{
	m_flag |= ImGuiWindowFlags_NoCollapse;
	m_windowClass = std::make_unique<ImGuiWindowClass>();

	Rename(widgetName);

	m_directoryBrowserButtonTexture = EditorContext::Instance().images->LoadImage(R"(Icons\OpenFolder.texture)");
	
	m_buildSavePath = INTERNAL_EDITOR_RESOURCES_ROOT + std::string(Build_Save_File);

	Deserialize();
}


BuildSettingsWindow::~BuildSettingsWindow()
{
	Serialize();
}

const BuildData& BuildSettingsWindow::GetBuildData() const
{
	return m_buildData;
}

void BuildSettingsWindow::Update()
{
	
}


void BuildSettingsWindow::Display()
{
	if (!isActive) return;

	GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
	bool opened = BeginWindow();
	GUI::PopStyle();

	if (opened)
	{

		ImVec2 dimension = ImGui::GetContentRegionAvail();
		if (ImGui::BeginChild("BuildSettings", { dimension.x, dimension.y * 0.9f }, true, ImGuiWindowFlags_AlwaysUseWindowPadding))
		{
			float panelWidth = ImGui::GetContentRegionAvail().x;

			if (ImGui::BeginTable("", 2, ImGuiTableFlags_SizingStretchProp))
			{
				ImGui::TableSetupColumn(ImGui::TableGetColumnName(0), ImGuiTableColumnFlags_NoResize, panelWidth * 0.25f);
				ImGui::TableSetupColumn(ImGui::TableGetColumnName(1), ImGuiTableColumnFlags_NoResize, panelWidth * 0.75f);

				ImGui::TableNextColumn();

				ImGui::Text("Build path");
				ImGui::TableNextColumn();

				{
					char* path = const_cast<char*>(m_buildData.buildPath.c_str());

					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
					if (ImGui::InputText("###BUILDPATH_INPUT", path, 256))
					{
						m_buildData.buildPath = path;
					}

					ImGui::SameLine();

					Color4 tintcolor = EditorContext::Instance().theme->color_interactives.defaultColor;
					ImVec4 tintvec = { tintcolor.r, tintcolor.g, tintcolor.b, tintcolor.a };

					ImVec2 iconDimension = { 30.f ,30.f };
					ImGui::SetCursorPosY(ImGui::GetCursorPos().y - (iconDimension.y - ImGui::GetFont()->FontSize) * 0.5f);

					GUI::PushColor(ImGuiCol_Button, { 0,0,0,0 });
					GUI::PushColor(ImGuiCol_ButtonHovered, { 0.f,0.f,0.f,0.05f });
					GUI::PushColor(ImGuiCol_ButtonActive, { 0.f,0.f,0.f,0.1f });
					if (ImGui::ImageButton(GUI::CastTextureID(m_directoryBrowserButtonTexture), iconDimension, ImVec2(0, 1), ImVec2(1, 0), -1, ImVec4(0, 0, 0, 0), tintvec))
					{
						std::string selectedPath = EditorContext::Instance().editorFileDialog->OpenDirectory(m_buildData.buildPath.c_str()).string();

						if (!selectedPath.empty())
						{
							m_buildData.buildPath = selectedPath + "\\";
						}
					}

					GUI::PopColor(3);
				}


				ImGui::TableNextColumn();

				ImGui::Text("Folder name");
				ImGui::TableNextColumn();

				char* name = const_cast<char*>(m_buildData.folderName.c_str());

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
				if (ImGui::InputText("###BUILDFOLDERNAME_INPUT", name, 56))
				{
					m_buildData.folderName = name;
				}

				{
					GUI::InspectPropertiesData propertiesData;
					bool isModified = GUI::DrawRef<Resource>("Start scene", &Scene::staticGetArchetype(), static_cast<void*>(&m_startScene), propertiesData);

					if (isModified && m_startScene != nullptr)
						m_buildData.startSceneUId = std::string(m_startScene->GetUID());
				}

				ImGui::SameLine();

				ImGui::EndTable();
			}
		}
		ImGui::EndChild();

		ImGui::Separator();

		float buttonSize = ImGui::CalcTextSize("Build & Run").x + ImGui::CalcTextSize("Build").x + ImGui::GetStyle().FramePadding.x * 4.f;


		ImGui::SetCursorPosX(dimension.x - buttonSize - ImGui::GetStyle().ItemSpacing.x);

		if (ImGui::Button("Build & Run"))
		{
			GameBuilder::BuildAndRunGame(m_buildData);
		}

		ImGui::SameLine();

		if (ImGui::Button("Build"))
		{
			if (!m_buildData.buildPath.empty())
			{
				GameBuilder::BuildGame(m_buildData);
			}
		}

	}
	EndWindow();
}

void BuildSettingsWindow::SerializeBuildFile(const std::string& path)
{
	GameContext::SaveSettings(path, m_buildData.startSceneUId);
}

void BuildSettingsWindow::Serialize()
{
	json jsonFile;

	// Save data
	json& jsonBuild = jsonFile["BuildData"];

	jsonBuild["BuildPath"] = m_buildData.buildPath;
	jsonBuild["BuildName"] = m_buildData.folderName;
	jsonBuild["IconPath"] = m_buildData.iconPath;
	jsonBuild["StartScene"] = m_buildData.startSceneUId;

	WriteJson(m_buildSavePath, jsonFile);
}

void BuildSettingsWindow::Deserialize()
{
	if (!std::filesystem::exists(m_buildSavePath))
		return;

	json jsonFile;
	ReadJson(m_buildSavePath, jsonFile);

	// Load data
	json& jsonBuild = jsonFile["BuildData"];
	
	Serialization::TryGetValue(jsonBuild, "BuildPath", m_buildData.buildPath);
	Serialization::TryGetValue(jsonBuild, "BuildName", m_buildData.folderName);
	Serialization::TryGetValue(jsonBuild, "IconPath", m_buildData.iconPath);
	Serialization::TryGetResource(jsonBuild, "StartScene", m_startScene);

	if (m_startScene != nullptr)
		m_buildData.startSceneUId = std::string(m_startScene->GetUID());
}

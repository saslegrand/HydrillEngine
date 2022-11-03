#include "Windows/ProjectSettingsWindow.hpp"

#include <filesystem>
#include <nlohmann/json.hpp>
#include <imgui/imgui.h>

#include <IO/Mouse.hpp>
#include <Core/Logger.hpp>
#include <EngineContext.hpp>
#include <IO/InputSystem.hpp>
#include <Tools/SerializationUtils.hpp>
#include <Renderer/RenderSystem.hpp>
#include <Resources/SceneManager.hpp>
#include <Resources/Resource/Scene.hpp>
#include <Resources/ResourcesManager.hpp>
#include <Renderer/RenderObjects/Camera.hpp>

#include "Widgets/WidgetManager.hpp"
#include "EditorContext.hpp"
#include "PathConfig.hpp"
#include "GUI.hpp"

constexpr char Project_Save_File[] = R"(Saves\\ProjectSettings.settings)";

ProjectSettingsWindow::ProjectSettingsWindow(const std::string& widgetName, const bool isActive)
	: EditorWindow(widgetName, isActive, "###PROJECT_SETTING")
{
	m_flag |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	m_windowClass = std::make_unique<ImGuiWindowClass>();

	Rename(widgetName);

	ProjectSettingCategory& inputManager = m_categories.emplace_back();

	inputManager.name = "Input Manager";
	inputManager.displayFunction = std::bind(&ProjectSettingsWindow::DisplayInputManager, this);

	ProjectSettingCategory& renderParameters = m_categories.emplace_back();

	renderParameters.name = "Render Parameters";
	renderParameters.displayFunction = std::bind(&ProjectSettingsWindow::DisplayRenderParameters, this);

	m_selectedCategory = &m_categories[0];

	projSavePath = INTERNAL_EDITOR_RESOURCES_ROOT + std::string(Project_Save_File);
}


ProjectSettingsWindow::~ProjectSettingsWindow()
{
}


void ProjectSettingsWindow::Update()
{
}


void ProjectSettingsWindow::Display()
{
	if (!isActive) return;

	GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
	bool opened = BeginWindow();
	GUI::PopStyle();

	if (opened)
	{
		float width = ImGui::GetContentRegionAvail().x;

		if (ImGui::BeginTable("###PROJECTSETTINGS_TABLE", 2, ImGuiTableFlags_SizingFixedFit))
		{
			ImGui::TableSetupColumn(ImGui::TableGetColumnName(0), 0, width * 0.25f);
			ImGui::TableSetupColumn(ImGui::TableGetColumnName(1), 0, width * 0.75f);

			ImGui::TableNextColumn();

			GUI::PushColor(ImGuiCol_ChildBg, EditorContext::Instance().theme->color_backgrounds.defaultColor);
			bool categoriesOpenend = ImGui::BeginChild("CATEGORY SELECTOR", ImGui::GetContentRegionAvail(), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
			GUI::PopColor();

			if (categoriesOpenend)
			{
				for (ProjectSettingCategory& s : m_categories)
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

struct KeyPairDisplayed
{
	std::unordered_map<std::string, Input::Button*>::iterator it;
	std::string tempName;
};

static void DrawKeyCode(const std::string& inputName, std::string& idName, int& id)
{
	char bufferId[256];
	strcpy_s(bufferId, IM_ARRAYSIZE(bufferId), idName.c_str());

	if (ImGui::InputText(inputName.c_str(), bufferId, IM_ARRAYSIZE(bufferId), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		rfk::Enum const* keyEnum = rfk::getDatabase().getFileLevelEnumByName("Key");
		rfk::EnumValue const* keyEnumValue = keyEnum->getEnumValueByName(bufferId);

		if (keyEnumValue == nullptr)
		{
			bufferId[0] = 0;
			id = -1;
		}
		else
		{
			id = static_cast<int>(keyEnumValue->getValue());
		}

	}

	idName = bufferId;
}


void ProjectSettingsWindow::DisplayRenderParameters()
{
	RenderSystem& renderSystem = SystemManager::GetRenderSystem();
	GraphicsSettings& param = renderSystem.graphicsSettings;

	float panelWidth = ImGui::GetContentRegionAvail().x;

	if (ImGui::BeginTable("", 2, ImGuiTableFlags_SizingStretchProp))
	{
		ImGui::TableSetupColumn(ImGui::TableGetColumnName(0), ImGuiTableColumnFlags_NoResize, panelWidth * 0.25f);
		ImGui::TableSetupColumn(ImGui::TableGetColumnName(1), ImGuiTableColumnFlags_NoResize, panelWidth * 0.75f);

		ImGui::TableNextColumn();


		//	--------

		ImGui::Text("Bloom Enabled");
		ImGui::TableNextColumn();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::Checkbox("###BloomEnabled", &param.bloomEnabled);
		ImGui::TableNextColumn();

		ImGui::Text("Bloom Threshold");
		ImGui::TableNextColumn();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::DragFloat("###BloomThreshold", &param.bloomThreshold, 0.05f, 0.f, 5.f);
		ImGui::TableNextColumn();

		//	--------

		ImGui::Text("Enable auto-exposure");
		ImGui::TableNextColumn();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::Checkbox("###AutoExposure", &param.autoExposure);
		ImGui::TableNextColumn();

		if (param.autoExposure)
		{
			//	--------

			ImGui::Text("	Brightness Calcul Frequency");
			ImGui::TableNextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::DragFloat("###BrigthnessCalculFrequency", &param.brightnessCalculFrequency, 0.1f, 0.0f, 5.f);
			ImGui::TableNextColumn();

			//	--------
			
			ImGui::Text("	Exposure Multiplier");
			ImGui::TableNextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::DragFloat("###ExposureMultiplier", &param.exposureMultiplier, 0.05f, 0.0f, 5.f);
			ImGui::TableNextColumn();

			//	--------

			ImGui::Text("	Exposure Speed");
			ImGui::TableNextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::DragFloat("###ExposureSpeed", &param.exposureSpeed, 0.05f, 0.0f, 5.f);
			ImGui::TableNextColumn();
			
			//	--------

			ImGui::Text("Exposure Range");
			ImGui::TableNextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::DragFloatRange2("###MinRange", &param.minExposure, &param.maxExposure, 0.005f, 0.0f, 25.f);
			ImGui::TableNextColumn();

		}
		else
		{
			//	--------

			ImGui::Text("	Constant Exposure");
			ImGui::TableNextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::DragFloat("###ConstantExposure", &param.constantExposure, 0.05f, 0.0f, 25.f);
			ImGui::TableNextColumn();

		}

		//	--------

		bool UpdateGPUSettings = false;

		ImGui::Text("Enable shadows");
		ImGui::TableNextColumn();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		UpdateGPUSettings = ImGui::Checkbox("###Shadows", &param.GPUSettings.shadows);
		ImGui::TableNextColumn();

		if (UpdateGPUSettings)
		{
			param.SendGPUSettings();
		}

		ImGui::EndTable();
	}
}

void  ProjectSettingsWindow::DisplayInputManager()
{
	if (ImGui::TreeNodeEx("Buttons"))
	{
		auto& namedButtonList = Keyboard::GetNamedButtonList();

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		for (int indexButton = 0; indexButton < namedButtonList.size(); indexButton++)
		{
			NamedButton& namedButton = namedButtonList[indexButton];

			std::string id = "Button" + std::to_string(indexButton);
			ImGui::PushID(id.c_str());

			char bufferName[256];
			strcpy_s(bufferName, IM_ARRAYSIZE(bufferName), namedButton.name.c_str());

			if (ImGui::InputText("Name", bufferName, IM_ARRAYSIZE(bufferName)))
				namedButton.name = bufferName;

			DrawKeyCode("Key name", namedButton.idName, namedButton.id);
			ImGui::PopID();

			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

			// DropDown Keycode here
		}

		if (ImGui::Button("Add new button"))
			Keyboard::AddKey("NewButton");

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Axes"))
	{
		auto& namedAxesList = Keyboard::GetNamedAxesList();

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		for (int indexAxis = 0; indexAxis < namedAxesList.size(); indexAxis++)
		{
			NamedAxis& namedAxis = namedAxesList[indexAxis];

			std::string id = "Axis" + std::to_string(indexAxis);
			ImGui::PushID(id.c_str());

			char bufferName[256];
			strcpy_s(bufferName, IM_ARRAYSIZE(bufferName), namedAxis.name.c_str());

			if (ImGui::InputText("Name", bufferName, IM_ARRAYSIZE(bufferName)))
				namedAxis.name = bufferName;

			DrawKeyCode("Neg name", namedAxis.idNameNeg, namedAxis.axis.buttonIdNeg);
			DrawKeyCode("Pos name", namedAxis.idNamePos, namedAxis.axis.buttonIdPos);

			ImGui::DragFloat("Amplitude", &namedAxis.axis.amplitude, 0.5f, 0.1f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragFloat("Time multiplier", &namedAxis.axis.timeMultiplier, 0.5f, 0.1f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);

			ImGui::PopID();

			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

			// DropDown Keycode here
		}

		if (ImGui::Button("Add new axis"))
			Keyboard::AddAxis("NewAxis");

		ImGui::TreePop();
	}
}



void ProjectSettingsWindow::Serialize()
{
	json j;

	Keyboard::SaveKeys(j);

	RenderSceneWidget& sceneWidget = EditorContext::Instance().widgetManager->GetSceneWidget();
	sceneWidget.SaveEditorCamera(j);

	GraphicsSettings& graphics = SystemManager::GetRenderSystem().graphicsSettings;
	graphics.Save(j);

	Scene* curScene = EngineContext::Instance().sceneManager->GetCurrentScene();
	j["Scene"] = curScene ? std::string(curScene->GetUID()) : std::string("0");

	WriteJson(projSavePath, j);
}

void ProjectSettingsWindow::Deserialize()
{
	if (!std::filesystem::exists(projSavePath))
	{
		EngineContext::Instance().sceneManager->LoadExistingSceneOrCreateDefault();
		return;
	}

	json j;
	ReadJson(projSavePath, j);

	Keyboard::LoadKeys(j);

	RenderSceneWidget& sceneWidget = EditorContext::Instance().widgetManager->GetSceneWidget();
	sceneWidget.LoadEditorCamera(j);

	GraphicsSettings& graphics = SystemManager::GetRenderSystem().graphicsSettings;
	graphics.Load(j);

	// Try load previous editor scene. If none, load another existing scene or create a new one
	Scene* lastOpenedScene = nullptr;
	if (Serialization::TryGetResource(j, "Scene", lastOpenedScene))
	{
		if (lastOpenedScene != nullptr)
		{
			EngineContext::Instance().sceneManager->ChangeCurrentScene(lastOpenedScene);
			return;
		}
	}

	EngineContext::Instance().sceneManager->LoadExistingSceneOrCreateDefault();
}
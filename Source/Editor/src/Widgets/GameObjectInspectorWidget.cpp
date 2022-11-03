#include "Widgets/GameObjectInspectorWidget.hpp"

#include <unordered_map>
#include <functional>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <Resources/Resource/Resource.hpp> 
#include <Resources/Resource/Mesh.hpp> 
#include <Resources/SceneManager.hpp> 
#include <ECS/GameObject.hpp> 
#include <ECS/Behavior.hpp> 
#include <Core/TaskQueue.hpp> 
#include <IO/Keyboard.hpp> 
#include <IO/Mouse.hpp> 
#include <Tools/StringHelper.hpp> 
#include <Types/Color.hpp>

#include <EngineContext.hpp>

#include "Tools/InspectorUtils.hpp"
#include "GUITypes.hpp"
#include "GUI.hpp"
#include "EditorContext.hpp"
#include "EditorSelectionManager.hpp"
#include "EditorDragManager.hpp"



GameObjectInspectorWidget::GameObjectInspectorWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive)
	: EditorWidget(widgetName, widgetID, isActive, "###GAMEOBJECT_INSPECTOR")
{
	m_flag |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysVerticalScrollbar;

	EditorContext::Instance().selectionManager->onGameObjectUpdated.AddCallback(&GameObjectInspectorWidget::GameObjectSelectionUpdate, *this);

	suggestion.researchedString = "";

	m_openedCadenasButtonTexture = EditorContext::Instance().images->LoadImage(R"(Icons\CadenasOpen.texture)");
	m_closedCadenasButtonTexture = EditorContext::Instance().images->LoadImage(R"(Icons\CadenasClose.texture)");
}

void GameObjectInspectorWidget::GameObjectSelectionUpdate(HYGUID* gameObject)
{
	if (m_lockCurrentGameObject) return;

	const HYGUID* newSelected = nullptr;
	if (gameObject == nullptr)
	{
		currentGameObject = HYGUID("0");
	}

	EditorSelectionManager* selecManager = EditorContext::Instance().selectionManager;

	if (selecManager->GetGameObjects().empty()) return;
	
	ImGui::SetWindowFocus(m_nameID.c_str());
	currentGameObject = selecManager->GetGameObjects()[0];
	GameObject* obj   = selecManager->GetGameObjectFromScene(currentGameObject);

	if (obj)
	{
		selecManager->selectedObjectPosition = obj->transform.LocalPosition();
		selecManager->selectedObjectRotation = obj->transform.LocalEulerAngles();
		selecManager->selectedObjectScale = obj->transform.LocalScale();
	}
	suggestion.researchedString = "";
}


int InputCallback(ImGuiInputTextCallbackData* data)
{
	SuggestionData& state = *reinterpret_cast<SuggestionData*>(data->UserData);


	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackHistory:

		//	Arrows selections
		if (data->EventKey == ImGuiKey_DownArrow)
		{
			state.activeIdx = Maths::Min(state.activeIdx + 1, static_cast<int>(state.suggestionList.size()) - 1);
		}
		else if (data->EventKey == ImGuiKey_UpArrow)
		{
			state.activeIdx = Maths::Max(state.activeIdx - 1, 0);
		}

		break;

	default: break;
	}

	return 0;
}

void GameObjectInspectorWidget::Display()
{
	if (!isActive) return;


	if (BeginWidget())
	{
		EditorSelectionManager* selecManager = EditorContext::Instance().selectionManager;
		
		GameObject* obj = nullptr;
		if (currentGameObject != HYGUID("0"))
		{
			obj = selecManager->GetGameObjectFromScene(currentGameObject);
		}

		if(obj)
		{
			const char* name = obj->GetName().c_str();

			ImVec2 texDimension = ImGui::CalcTextSize(name);

			EditorContext::Instance().theme->defaultFont->UseBold();

			ImGui::AlignTextToFramePadding();

			float width = ImGui::GetContentRegionAvail().x;
			ImGui::SetCursorPosX(width * 0.5f - texDimension.x * 0.5f);
			ImGui::Text(name);

			ImGui::SameLine();

			ImVec2 iconDimension = { 20.f ,20.f };

			ImGui::SetCursorPosX(width - 10.f - texDimension.y * 1.25f);
			ImGui::SetCursorPosY(ImGui::GetCursorPos().y - (iconDimension.y - ImGui::GetFont()->FontSize) * 0.5f);

			unsigned int tex = m_lockCurrentGameObject ? m_closedCadenasButtonTexture : m_openedCadenasButtonTexture;

			GUI::PushColor(ImGuiCol_Button, { 0.0f,0.0f,0.0f,0.0f });
			GUI::PushColor(ImGuiCol_ButtonHovered, { 0.0f,0.0f,0.0f,0.05f });
			GUI::PushColor(ImGuiCol_ButtonActive, { 0.0f,0.0f,0.0f,0.1f });
			bool clicked = ImGui::ImageButton(GUI::CastTextureID(tex), iconDimension, ImVec2(0, 1), ImVec2(1, 0), -1);
			GUI::PopColor(3);

			EditorSelectionManager* selection = EditorContext::Instance().selectionManager;

			if (clicked)
			{
				m_lockCurrentGameObject = !m_lockCurrentGameObject;

				if (!m_lockCurrentGameObject && !EditorContext::Instance().selectionManager->GetGameObjects().empty())
				{
					currentGameObject = EditorContext::Instance().selectionManager->GetGameObjects()[0];
					selection->selectedObjectPosition = obj->transform.LocalPosition();
					selection->selectedObjectRotation = obj->transform.LocalEulerAngles();
					selection->selectedObjectScale = obj->transform.LocalScale();
				}
			}

			EditorContext::Instance().theme->defaultFont->EndFontStyle();

			ImGui::Separator();

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Is static");
			ImGui::SameLine();
			ImGui::Checkbox("###STATIC_CHECKBOX", &obj->isStatic);

			ImGui::Text("Is active");
			ImGui::SameLine();
			bool isActive = obj->IsSelfActive();
			if (ImGui::Checkbox("###ACTIVE_CHECKBOX", &isActive))
				obj->SetActive(isActive);

			ImGui::Spacing();

			GUI::PushStyle(ImGuiStyleVar_FrameRounding, 2.5f);

			EditorContext::Instance().theme->defaultFont->UseBold();
			ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_Leaf);
			EditorContext::Instance().theme->defaultFont->EndFontStyle();

			GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
			bool transformContextMenu = ImGui::BeginPopupContextItem("###TRANSFORM_CONTEXTMENU");
			GUI::PopStyle();

			if (transformContextMenu)
			{
				if (ImGui::MenuItem("Reset transform"))
				{
					obj->transform.SetLocalPosition(Vector3::Zero);
					obj->transform.SetLocalRotation(Quaternion::Identity);
					obj->transform.SetLocalScale(Vector3::One);

					selection->selectedObjectPosition = Vector3::Zero;
					selection->selectedObjectRotation = Vector3::Zero;
					selection->selectedObjectScale = Vector3::One;
				}

				ImGui::EndPopup();
			}

			GUI::InspectPropertiesData propertiesData;
			if (GUI::DrawFundamental<Vector3>("Position", selection->selectedObjectPosition.elements, propertiesData))
			{
				obj->transform.SetLocalPosition(selection->selectedObjectPosition);
			}

			if (GUI::DrawFundamental<Vector3>("Rotation", selection->selectedObjectRotation.elements, propertiesData))
			{
				obj->transform.SetLocalEulerAngles(selection->selectedObjectRotation);
			}

			if (GUI::DrawFundamental<Vector3>("Scale", selection->selectedObjectScale.elements, propertiesData))
			{
				obj->transform.SetLocalScale(selection->selectedObjectScale);
			}


			ImGui::Separator();

			AddComponentBrowser(*obj);

			ImGui::Separator();
			ImGui::Spacing();

			// Display of current GameObject components
			DisplayComponents(*obj);

			GUI::PopStyle();
		}
	}
	EndWidget();
}


void GameObjectInspectorWidget::AddComponentBrowser(GameObject& currentObj)
{
	ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_CallbackHistory;

	if (suggestion.regen) GenerateSuggestions();

	ImGui::AlignTextToFramePadding();

	float panelWidth = ImGui::GetContentRegionAvail().x;

	if (ImGui::BeginTable("###Table", 2, ImGuiTableFlags_SizingStretchProp))
	{
		ImGui::TableSetupColumn(ImGui::TableGetColumnName(0), ImGuiTableColumnFlags_NoResize, panelWidth * 0.25f);
		ImGui::TableSetupColumn(ImGui::TableGetColumnName(1), ImGuiTableColumnFlags_NoResize, panelWidth * 0.75f);

		ImGui::TableNextColumn();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Add component");

		ImGui::SameLine(0, 50.f);

		ImGui::TableNextColumn();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

		char* searchedComponent = const_cast<char*>(suggestion.researchedString.c_str());

		if (ImGui::InputText("###AddComponent", searchedComponent, 50, inputTextFlags, InputCallback, &suggestion))
		{
			suggestion.regen = true;
			suggestion.researchedString = std::string(searchedComponent);
		}
		suggestion.isPopupOpen |= ImGui::IsItemActive();

		SuggestionPopup(currentObj);

		ImGui::EndTable();
	}
}


void GameObjectInspectorWidget::SuggestionPopup(GameObject& currentObj)
{
	if (!suggestion.isPopupOpen) return;

	//	Flag of suggestion popup
	static ImGuiWindowFlags popupFlag = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoSavedSettings;

	//	Get transforms of suggestion popup

	ImVec2 popupPos = ImGui::GetItemRectMin();

	float height = ImGui::CalcTextSize("H", NULL, true).y + ImGui::GetStyle().ItemSpacing.y;

	popupPos.y += ImGui::GetItemRectSize().y;

	float popupSize = ImGui::GetItemRectSize().x;

	//	Limit the height of the popup
	ImVec2 size = { popupSize, Maths::Clamp(suggestion.suggestionList.size() * height, height, 10 * height) };

	//	Set popup position and dimensions
	ImGui::SetNextWindowPos(popupPos);
	ImGui::SetNextWindowSize(size);

	bool isFocused = ImGui::IsItemFocused();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1, 1));

	if (ImGui::Begin("input_popup", nullptr, popupFlag))
	{
		ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
		isFocused |= ImGui::IsWindowFocused();

		if (!ImGui::IsWindowHovered() && Input::Mouse::WasClicked()) isFocused = false;

		for (int i = 0; i < suggestion.suggestionList.size(); i++)
		{
			bool isIndexActive = suggestion.activeIdx == i;

			ImGui::PushID(i);
			if (ImGui::Selectable(suggestion.suggestionList[i].c_str(), isIndexActive) || (isIndexActive && Input::Keyboard::GetKeyPressed(Key::Enter)))
			{
				currentObj.AddComponentFromName(suggestion.suggestionList[i]);
				suggestion.isPopupOpen = false;
				suggestion.activeIdx = -1;
			}
			ImGui::PopID();
		}
	}

	ImGui::PopStyleVar(1);
	ImGui::End();
	suggestion.isPopupOpen &= isFocused;
}

void GameObjectInspectorWidget::DisplayComponents(GameObject& currentObj)
{
	for (Component* comp : currentObj.GetRawComponents())
	{
		if (comp != nullptr)
		{
			ImGui::Spacing();

			bool visible = true;

			ImVec2 savedPos = ImGui::GetCursorPos();
			float width = ImGui::GetContentRegionAvail().x;

			EditorContext::Instance().theme->defaultFont->UseBold();
			bool opened = ImGui::CollapsingHeader(comp->getArchetype().getName(), &visible);
			EditorContext::Instance().theme->defaultFont->EndFontStyle();

			float framePadding = 1.f;
			GUI::PushStyle(ImGuiStyleVar_FramePadding, ImVec2(framePadding, framePadding));

			ImGui::SetItemAllowOverlap();
			ImGui::SetCursorPosX(savedPos.x + width - width * 0.1f - ImGui::GetFontSize() + framePadding * 2.f);
			ImGui::SetCursorPosY(savedPos.y + (ImGui::GetFontSize() + framePadding) * 0.25f);

			bool isActive = comp->IsSelfActive();
			if (ImGui::Checkbox("###IsActive", &isActive))
			{
				comp->SetActive(isActive);
			}
			GUI::PopStyle();

			//	Close / Delete button on collapsing header
			if (!visible)
			{
				EditorContext::Instance().taskQueue->AddTask([&currentObj, comp]()
					{
						currentObj.DestroyComponent(*comp);
					});
			}

			//	Display content of the component
			else if (opened)
			{
				ImGui::Spacing();

				bool isModified = InspectorMethods::DisplayEntity(comp, &comp->getArchetype());

				if (isModified)
					comp->OnModify();
			}
		}
	}
}


void GameObjectInspectorWidget::GenerateSuggestions()
{
	suggestion.suggestionList.clear();
	{
		rfk::getDatabase().foreachFileLevelClass([](rfk::Class const& c, void* data) {

			if (c.isSubclassOf(Component::staticGetArchetype()) && !c.isBaseOf(Behavior::staticGetArchetype()))
			{
				//	Particular abstract components 
				//	TODO : Make Abstract field in HYCLASS() to avoid doing this

				if (std::string(c.getName()) == "Collider") return true;

				SuggestionData& param = *static_cast<SuggestionData*>(data);

				if (StringHelper::StrStrCaseUnsensitive(c.getName(), param.researchedString))
					param.suggestionList.push_back(c.getName());
			}

			return true;
			}, &suggestion);
	}
	suggestion.regen = false;
}

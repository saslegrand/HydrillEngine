#include "Widgets/ToolbarWidget.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glad/gl.h>

#include <EngineContext.hpp>
#include <Resources/ResourcesManager.hpp>
#include <Resources/SceneManager.hpp>
#include <DllWrapper.hpp>

#include "EditorContext.hpp"
#include "EditorSelectionManager.hpp"
#include "GUI.hpp"

ToolbarWidget::ToolbarWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive)
	: EditorWidget(widgetName, widgetID, isActive, "###TOOLBAR")
{
	m_flag = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus;
	m_windowClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe;
	m_windowClass->TabItemFlagsOverrideSet  = ImGuiTabItemFlags_NoCloseWithMiddleMouseButton;

	m_playButtonTexture		= EditorContext::Instance().images->LoadImage(R"(Icons\PlayButton.texture)");
	m_pauseButtonTexture	= EditorContext::Instance().images->LoadImage(R"(Icons\PauseButton.texture)");
	m_stopButtonTexture		= EditorContext::Instance().images->LoadImage(R"(Icons\StopButton.texture)");
	m_compileButtonTexture  = EditorContext::Instance().images->LoadImage(R"(Icons\CompileButton.texture)");
}


bool DisplayButton(unsigned int texture, GameState state, ImVec2 dim)
{
	ImVec4 bg   = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	ImVec4 tint = ImVec4(0.75f, 0.75f, 0.75f, 0.99f);

	if (EditorContext::Instance().gameState == state)
	{
		Color4 tintCol = EditorContext::Instance().theme->color_interactives.defaultColor;

		tint = ImVec4(tintCol.r, tintCol.g, tintCol.b, tintCol.a);
	}

	return ImGui::ImageButton(GUI::CastTextureID(texture), dim, ImVec2(0, 1), ImVec2(1, 0), 0, bg, tint);
}

bool ToolbarWidget::BeginWidget()
{
	ImGui::SetNextWindowClass(m_windowClass.get());
	bool opened = ImGui::Begin(m_nameID.c_str(), nullptr, m_flag);
	return opened;
}

void ToolbarWidget::Display()
{
	if (!isActive) return;

	EditorTheme* t = EditorContext::Instance().theme;

	GUI::PushColor(ImGuiCol_WindowBg, t->color_borders.defaultColor);
	GUI::PushStyle(ImGuiStyleVar_WindowPadding, ImVec2(2.f, 2.f));

	bool opened = BeginWidget();
	
	GUI::PopStyle();
	GUI::PopColor();

	if (opened)
	{
		GUI::PushColor(ImGuiCol_Button, t->color_borders.defaultColor);
		GUI::PushColor(ImGuiCol_ButtonHovered, t->color_borders.hoveredColor);
		GUI::PushColor(ImGuiCol_ButtonActive, t->color_borders.activeColor);

		float spacing = ImGui::GetStyle().ItemSpacing.x + 5.f;
		ImVec2 dimensions = ImGui::GetContentRegionAvail();

		ImGui::SetCursorPosX(dimensions.x * 0.5f - (dimensions.y + spacing) * 3 * 0.5f);

		if (DisplayButton(m_playButtonTexture, GameState::Play, ImVec2(dimensions.y, dimensions.y)))
		{
			EditorContext::Instance().onEnginePlayed.Invoke();
		}

		ImGui::SameLine(0, spacing);

		if (DisplayButton(m_pauseButtonTexture, GameState::Pause, ImVec2(dimensions.y, dimensions.y)))
		{
			EditorContext::Instance().onEnginePaused.Invoke();
		}

		ImGui::SameLine(0, spacing);

		if (DisplayButton(m_stopButtonTexture, GameState::Sleep, ImVec2(dimensions.y, dimensions.y)))
		{
			EditorContext::Instance().onEngineStopped.Invoke();
		}

		ImGui::SameLine(0, spacing);

		ImVec4 bg = ImVec4(0, 0, 0, 0);
		ImVec4 tint = ImVec4(0.75f, 0.75f, 0.75f, 0.99f);

		if (ImGui::ImageButton(GUI::CastTextureID(m_compileButtonTexture), ImVec2(dimensions.y, dimensions.y), ImVec2(0, 1), ImVec2(1, 0), 0, bg, tint))
		{
			EditorContext& editorContext = EditorContext::Instance();
			if (editorContext.gameState == GameState::Sleep)
			{
				Scene* currentScene = EngineContext::Instance().sceneManager->GetCurrentScene();
				if (currentScene != nullptr)
				{
					Logger::Info("Start Scripting Hot-Reload");
					Logger::Info("Reload active Scene and Scripting dll");
					// Save the scene
					currentScene->Serialize();
					// Unload all scene entities
					currentScene->Unload();
					// Reload the scripting dll
					editorContext.scriptingWapper->Reload();
					// Load current scene
					currentScene->Load();

					Logger::Info("Successfully complete Scripting Hot-Reload");
				}
				else
					Logger::Error("Impossible to Hot-Reload, there's currently no active scene");
			}
			else
				Logger::Warning("Impossible to Hot-Reload in Play/Pause modes");
		}
		GUI::PopColor(3);
	}
	EndWidget();
}
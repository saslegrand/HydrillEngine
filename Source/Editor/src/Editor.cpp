#include "Editor.hpp"

#include <imgui/imgui.h>
#include <fstream>

#include <EngineContext.hpp>
#include <Core/TaskQueue.hpp>
#include <Core/Time.hpp>
#include <IO/Mouse.hpp>
#include <IO/Keyboard.hpp>
#include <IO/Window.hpp>
#include <Resources/SceneManager.hpp>
#include <Resources/Loaders/ResourcesLoader.hpp>
#include <ECS/Systems/ParticleSystem.hpp>

#include "EditorContext.hpp"
#include "Tools/GameBuilder.hpp"
#include "EditorSelectionManager.hpp"
#include "EditorResourcePreview.hpp"
#include "EditorData.hpp"
#include "EditorDragManager.hpp"
#include "EditorImages.hpp"
#include "EditorTheme.hpp"
#include "Tools/FileDialogs.hpp"
#include "Widgets/WidgetManager.hpp"
#include "Windows/WindowManager.hpp"
#include "GUI.hpp"


Editor::Editor(Window* window)
{
    EditorContext::instance = &m_context;

    m_fileDialog = std::make_unique<FileDialogs>();
    m_fileDialog->Initialize(window);

    m_context.window = window;

    GUI::LaunchGUI(window);
}


Editor::~Editor()
{
    m_windowManager->SavePreferences();
    m_windowManager->SaveProjectSettings();

    GUI::ShutDownGUI();
}

void Editor::Initialize()
{
    ResourcesLoader::LoadResourcesInDirectory(INTERNAL_EDITOR_RESOURCES_ROOT);

    m_theme = std::make_unique<EditorTheme>();
    m_images = std::make_unique<EditorImages>();
    m_resourcePreview = std::make_unique<EditorResourcePreview>();
    m_selectionManager = std::make_unique<EditorSelectionManager>();
    m_data = std::make_unique<EditorData>();
    m_dragManager = std::make_unique<EditorDragManager>();
    m_taskQueue = std::make_unique<TaskQueue>();

    m_context.theme = m_theme.get();
    m_context.images = m_images.get();
    m_context.resourcePreview = m_resourcePreview.get();
    m_context.dragManager = m_dragManager.get();
    m_context.selectionManager = m_selectionManager.get();
    m_context.editorDatas = m_data.get();
    m_context.editorFileDialog = m_fileDialog.get();
    m_context.taskQueue = m_taskQueue.get();

    m_resourcePreview->Initialize();
    m_theme->InitializeColors();
    m_theme->SetColors();

    m_mainDockSpace = std::make_unique<GUI::Dockspace>("MainDockWindow", "DockSpaceID_01");

    m_windowManager = std::make_unique<WindowManager>();

    m_widgetManager = std::make_unique<WidgetManager>();
    m_widgetManager->AddWidget(WidgetTypes_Console, "Console");
    m_widgetManager->AddWidget(WidgetTypes_ToolBar, "ToolBar");
    m_widgetManager->AddWidget(WidgetTypes_Game, "Game");
    m_widgetManager->AddWidget(WidgetTypes_SceneGraph, "Scene Graph");
    m_widgetManager->AddWidget(WidgetTypes_ContentBrowser, "Content Browser");
    m_widgetManager->AddWidget(WidgetTypes_GameObjectInspector, "GameObject Inspector");
    m_widgetManager->AddWidget(WidgetTypes_ResourceInspector, "Resource Inspector");
    m_widgetManager->AddWidget(WidgetTypes_Scene, "Scene");

    // Set widget manager
    m_context.widgetManager = m_widgetManager.get();
    m_context.windowManager = m_windowManager.get();

    m_windowManager->LoadPreferences();
    m_windowManager->LoadProjectSettings();
}

void Editor::Update()
{
    m_widgetManager->UpdateWidgets();
    m_windowManager->UpdateWindows();
    m_resourcePreview->Update();

    //  Update that occur only when game is stopped
    if (EditorContext::Instance().gameState == GameState::Sleep)
    {
        EngineContext& engineContext = EngineContext::Instance();
        ParticleSystem& particleSystemPreview = SystemManager::GetParticleSystem();
        Time& time = *engineContext.time;

        float tick = time.GetUnscaledDeltaTime();

        particleSystemPreview.UpdateAll(tick);

        static float stocker = 0;
        stocker += tick;
        while (stocker > time.GetFixedDeltaTime())
        {
            // Fixed Update

            particleSystemPreview.FixedUpdateAll(time.GetFixedDeltaTime());
            stocker -= time.GetFixedDeltaTime();
        }
    }

    if (Input::Keyboard::GetKeyDown(Key::LeftControl) || Input::Keyboard::GetKeyDown(Key::RightControl))
    {
        if (Input::Keyboard::GetKeyPressed(Key::S))
        {
            Logger::Info("Saving current scene");
            EngineContext::Instance().sceneManager->SaveCurrentScene();
        }
    }

    // Invoke tasks
    m_taskQueue->InvokeTasks();
}

void Editor::Display()
{
    if (m_mainDockSpace->Begin())
    {
        //  Top MenuBar of docking space
        DisplayMenuBar();

        if (ImGui::Begin("DevWindow"));
        {
            static Vector2 minMax = Vector2(FLT_MAX, -FLT_MAX);

            float fps = EngineContext::Instance().time->GetFPS();
            if (fps < minMax.x)
                minMax.x = fps;
            if (fps > minMax.y)
                minMax.y = fps;

            ImGui::Text("FPS : %f", fps);
            ImGui::Text("Min : %f | Max : %f", minMax.x, minMax.y);

            if (ImGui::Button("Reset"))
                minMax = Vector2(FLT_MAX, -FLT_MAX);

            ImGui::Separator();
        }
        ImGui::End();

        m_widgetManager->DisplayWidgets();
        m_mainDockSpace->End();
    }

    m_windowManager->DisplayWindows();


    if (Input::Mouse::GetButtonUp(MouseButton::Left))
    {
        m_context.dragManager->EndDrag();
    }

}


void Editor::FrameStart()
{
    GUI::NewFrameGUI();
}

void Editor::Render()
{
    //  Display End
    GUI::RenderGUI();
}


//  Dockspace Functions
//  -------------------


void Editor::DisplayMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Project", nullptr))
            {
            }
            
            if (ImGui::MenuItem("Open Project", nullptr))
            {
                std::string projectPath = m_fileDialog->OpenFile("Hydrill project (*.Hydrill)\0*.Hydrill\0").string();

                if (!projectPath.empty())
                {
                    // Do something
                }
            }

            if (ImGui::MenuItem("Save Project", "CTRL+S"))
            {
                std::string projectPath = m_fileDialog->SaveFile("Hydrill project (*.Hydrill)\0*.Hydrill\0").string();

                if (!projectPath.empty())
                {
                    // Do something
                }
            }
            
            if (ImGui::MenuItem("Properties", nullptr))
            {

            }
            
            if (ImGui::MenuItem("Exit", nullptr))
            {
                shutdown = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("Preference", nullptr))
            {
                m_windowManager->SetActivePreferences(true);
            }

            if (ImGui::MenuItem("Project Settings", nullptr))
            {
                m_windowManager->SetActiveProjectSettings(true);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Build"))
        {
            if (ImGui::MenuItem("Build settings", nullptr))
            {
                m_windowManager->SetActiveBuildSettings(true);
            }
            if (ImGui::MenuItem("Build", nullptr))
            {
                GameBuilder::BuildGame(EditorContext::Instance().windowManager->GetBuildData());
            }
            if (ImGui::MenuItem("Build and Run", nullptr))
            {
                GameBuilder::BuildAndRunGame(EditorContext::Instance().windowManager->GetBuildData());
            }

            ImGui::EndMenu();
        }

        m_widgetManager->WidgetMenuBar();

        ImGui::EndMenuBar();
    }
}

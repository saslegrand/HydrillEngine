#include <Windows/WindowManager.hpp>

#include <string>

#include <imgui/imgui.h>

#include <EditorContext.hpp>
#include <EditorTheme.hpp>

WindowManager::WindowManager()
{

}

WindowManager::~WindowManager()
{

}

void WindowManager::UpdateWindows()
{
	buildSettingWindow.Update();
	preferenceWindow.Update();
	projectSettingWindow.Update();
}

void WindowManager::DisplayWindows()
{
	buildSettingWindow.Display();
	preferenceWindow.Display();
	projectSettingWindow.Display();
}


void WindowManager::SetActiveBuildSettings(bool active)
{
	buildSettingWindow.isActive = active;
}


void WindowManager::SetActivePreferences(bool active)
{
	preferenceWindow.isActive = active;
}


void WindowManager::SetActiveProjectSettings(bool active)
{
	projectSettingWindow.isActive = active;
}

void WindowManager::LoadPreferences()
{
	preferenceWindow.Deserialize();
}

void WindowManager::LoadProjectSettings()
{
	projectSettingWindow.Deserialize();
}

void WindowManager::SavePreferences()
{
	preferenceWindow.Serialize();
}

void WindowManager::SaveProjectSettings()
{
	projectSettingWindow.Serialize();
}

void WindowManager::SaveBuildSettingsForBuild(const std::string& path)
{
	buildSettingWindow.SerializeBuildFile(path);
}

const BuildData& WindowManager::GetBuildData() const
{
	return buildSettingWindow.GetBuildData();
}
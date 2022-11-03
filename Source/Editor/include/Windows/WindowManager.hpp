#pragma once

#include "Windows/BuildSettingsWindow.hpp"
#include "Windows/PreferencesWindow.hpp"
#include "Windows/ProjectSettingsWindow.hpp"

/**
@brief This class Manage all widgets of the editor
*/
class WindowManager
{

//	Variables

private:

	BuildSettingsWindow		buildSettingWindow;
	PreferencesWindow		preferenceWindow;
	ProjectSettingsWindow	projectSettingWindow;

//	Constructor(s) & Destructor(s)

public:

	WindowManager();
	~WindowManager();

//	Variables

public:

	/**
	@brief Display all the widgets
	*/
	void DisplayWindows();

	/**
	@brief Update all the widgets
	*/
	void UpdateWindows();

	void SetActiveBuildSettings(bool active);
	void SetActivePreferences(bool active);
	void SetActiveProjectSettings(bool active);

	/** @brief Load project preferences */
	void LoadPreferences();
	/** @brief Load project settings */
	void LoadProjectSettings();
	/** @brief Save project preferences */
	void SavePreferences();
	/** @brief Load project settings */
	void SaveProjectSettings();

	/** @brief Create build settings file in the build directory */
	void SaveBuildSettingsForBuild(const std::string& path);

	/** 
	@brief Return the build info for the GameBuilder

	@return BuildData& : Build info
	*/
	const BuildData& GetBuildData() const;
};

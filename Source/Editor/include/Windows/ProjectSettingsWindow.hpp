#pragma once

#include <string>
#include <vector>
#include <functional>

#include <Types/Serializable.hpp>

#include "Windows/EditorWindow.hpp"


/**
* @brief This is the project setting window which contains setting about current project
*/
class ProjectSettingsWindow : public EditorWindow, public Serializable
{


private:

	struct ProjectSettingCategory
	{
		std::string name;
		std::function<void()> displayFunction;
	}; 

//	Variables

private:

	std::vector<ProjectSettingCategory> m_categories;
	ProjectSettingCategory* m_selectedCategory;
	std::string projSavePath;

//	Constructor(s) & Destructor(s)

public:

	ProjectSettingsWindow(const std::string& widgetName = "Project settings", const bool isActive = false);
	virtual ~ProjectSettingsWindow();

//	Functions

private:

	void DisplayInputManager();
	void DisplayRenderParameters();

public:


	/**
	* @brief Window update function
	*/
	void Update() override;

	/**
	* @brief Window display function
	*/
	void Display() override;

	/**
	@brief Save project settings
	*/
	void Serialize() override;

	/**
	@brief Load project settings
	*/
	void Deserialize() override;
};
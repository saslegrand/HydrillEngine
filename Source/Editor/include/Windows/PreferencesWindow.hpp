#pragma once

#include <string>
#include <vector>
#include <functional>

#include <Types/Serializable.hpp>

#include "Windows/EditorWindow.hpp"

/**
* @brief This is the preferences window which contains setting on editor
*/
class PreferencesWindow : public EditorWindow, public Serializable
{

private:

	struct PreferenceCategory
	{
		std::string name;
		std::function<void()> displayFunction;
	};

//	Variables

private:

	std::vector<PreferenceCategory> m_categories;
	PreferenceCategory* m_selectedCategory;
	std::string prefSavePath;

public:

//	Constructor(s) & Destructor(s)

public:

	PreferencesWindow(const std::string& widgetName = "Preferences", const bool isActive = false);
	virtual ~PreferencesWindow();

//	Functions

private:

	void DisplayContentBrowserPreferences();
	void DisplayThemePreferences();

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
	@brief Save project preferences
	*/
	void Serialize() override;

	/**
	@brief Load project preferences
	*/
	void Deserialize() override;
};
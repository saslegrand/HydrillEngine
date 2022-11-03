#pragma once

#include <string>

#include <Types/Color.hpp>

/**
* @brief
*/
class EditorData
{

private :

	struct ContentBrowserPreferenceData
	{
		float fileThumbnailSize = 128.f;
		float filePadding = 16.f;
	};

	struct BuildSettingsData
	{
		std::string path;
	};

	struct SavedThemeData
	{
		Color4 backgrounds = { 0.13f, 0.13f, 0.16f, 1.f };
		Color4 borders = { 0.09f, 0.09f, 0.11f, 1.f };
		Color4 buttons = { 0.15f, 0.15f, 0.18f, 1.f };
		Color4 interactiveColor = { 0.2f, 0.3f, 0.4f, 0.98f };
		Color4 interactiveBackgroundColor = { 0.08f, 0.08f, 0.10f, 1.f };
	};

public:

	ContentBrowserPreferenceData contentBrowser;
	BuildSettingsData	buildSettings;
	SavedThemeData      themeData;
};

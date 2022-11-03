#pragma once

#include <memory>
#include <string>

#include <Types/Serializable.hpp>

#include "Windows/EditorWindow.hpp"
#include "PathConfig.hpp"

class Scene;

struct BuildData
{
	std::string buildPath = R"(.\)";
	std::string folderName = "HydrillBuild";
	std::string iconPath = "";
	std::string startSceneUId = "0";
};

/**
@brief This is the build setting window used to set build settings
*/
class BuildSettingsWindow : public EditorWindow, public Serializable
{

//	Variables

private:
	unsigned int m_directoryBrowserButtonTexture = 0;

	Scene* m_startScene = nullptr;

	std::string m_buildSavePath;
	BuildData   m_buildData;
	
public:


//	Constructor(s) & Destructor(s)

public:

	BuildSettingsWindow(const std::string& widgetName = "Build settings", const bool isActive = false);
	virtual ~BuildSettingsWindow();

//	Functions

public:
	const BuildData& GetBuildData() const;

	/**
	@brief Window update function
	*/
	void Update() override;

	/**
	@brief Window display function
	*/
	void Display() override;

	/**
	@brief Create the build settings file in the build folder

	@param path : Path where to save the settings fils int the build folder
	*/
	void SerializeBuildFile(const std::string& path);

	/**
	@brief Save build settings
	*/
	void Serialize() override;

	/**
	@brief Load build settings
	*/
	void Deserialize() override;
};
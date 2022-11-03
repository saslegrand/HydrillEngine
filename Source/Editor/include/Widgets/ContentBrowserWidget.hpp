#pragma once

#include <vector>
#include <functional>

#include "PathConfig.hpp"
#include "Widgets/EditorWidget.hpp"

//	Forward declaration
namespace std::filesystem
{
	class path;
}

class Resource;
struct PimplContentBrowser;
struct File;
struct Directory;

/**
@brief This widget allow the user to interact with scripts and resources from the content folder
It direclty shows files from the file explorer and for resourecs it pass through the resources manager
*/
class ContentBrowserWidget : public EditorWidget
{

//	Variables

private:

	std::unique_ptr<PimplContentBrowser> m_pimpl;

//	Constructor(s) & Destructor(s)

public:

	ContentBrowserWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive = false);
	~ContentBrowserWidget();

//	Functions

private:

	/**
	@brief Content browser navigation bar
	*/
	void DisplayNavigationBar();

	/**
	@brief Display content of the selected folder
	*/
	void DisplayContent();

	/**
	@brief Display directories recursively for the directory explorer

	@param dir: directory to display
	*/
	void DisplayDirectoryTree(Directory& dir);

	/**
	@brief Display directory tree explorer at the left of the content browser
	*/
	void DisplayFolderExplorer();

	/**
	@brief Display files

	@param file: file to display
	@param ID: file index in current directory
	*/
	void DisplayFile(File& file, const int ID);

	/**
	@brief Display directory

	@param dir: directory to display
	@param ID: directory index in current directory
	*/
	bool DisplayDirectory(Directory& dir, const int ID);

	/**
	@brief Delete confirmation popup

	@param deleteFunction: action to execute if the delete is confirmed
	@param message: message to show for on the confirmation popup
	*/
	void DeleteConfirm(const std::function<void()>& deleteFunction, const char* message = "Delete this file ?");

	/**
	@brief Delete directory function which is a recursive function

	@param dirPath: path of the directory to delete
	*/
	void DeleteDirectory(const std::filesystem::path& dirPath);

	/**
	@brief Reload directory tree under the given directory

	@param directory : directory to load
	*/
	void LoadDirectoryTree(Directory& directory);

	/**
	@brief Unload directory tree under the given directory

	@param directory : directory to unload
	*/
	void UnloadDirectoryTree(Directory& directory);

	/**
	@brief Load actie directory
	*/
	void LoadActiveDirectory();

	/**
	@brief Check and reload unloaded resources from the assets folder
	*/
	void CheckResourceToReload();

	/**
	@brief Change active directory to the directory with the given path

	@param directory : new active directory path
	*/
	void ChangeDirectory(const std::filesystem::path& directory);

	/**
	@brief Creation confirmation popup

	@param popupID : popup ID
	@param message : message to show for on the popup
	@param defaultFilename : default name of the new file
	@param createFunction : action to execute if creation was confirmed
	*/
	void DisplayCreationPopup(const char* popupID, const char* message, const std::string& defaultFilename, const std::function<void(const std::string&)>& createFunction);

public:

	/**
	* @brief Widget update function
	*/
	void Update() override;

	/**
	* @brief Widget display function
	*/
	void Display() override;
};

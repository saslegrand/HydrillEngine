#include <string>
#include <filesystem>

class Window;

class FileDialogs
{
//	Variables

private:

	Window* m_window;
//	Functions

public:

	/**
	@brief Initialize file dialog class

	@param window : need to be given so we can open child dialogs from the application window
	*/
	void Initialize(Window* window);

	/**
	@brief Open a file browser in which you can open a file

	@param filter : extension filter
	*/
	std::filesystem::path OpenFile(const char* filter);


	/**
	@brief Open a file browser in which you save a file

	@param filter : extension filter
	*/
	std::filesystem::path SaveFile(const char* filter);

	/**
	@brief Open directory browser

	@param currentDir : current directory
	*/
	std::filesystem::path OpenDirectory(const char* currentDir);

};
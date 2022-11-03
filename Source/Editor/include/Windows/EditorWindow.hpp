#pragma once

#include <memory>
#include <string>

#include "EditorTheme.hpp"

#define WINDOWNAME_CHAR_MAX 28

/**
* @brief This is the parent class of all windows
*/
class EditorWindow
{

//	Variables

protected:

	std::unique_ptr<struct ImGuiWindowClass> m_windowClass = nullptr;

	int  m_flag = 0;
	std::string	 m_nameID;
	std::string  m_name = "window";

	const std::string m_ID;
	bool m_externViewport = false;

	bool initializeSize = true;

public:

	bool isActive = false;

	//	Constructor(s) & Destructor(s)

public:

	EditorWindow(const std::string& windowName = "Window", const bool isActive = false, const std::string& typeID = "###WINDOW");
	virtual ~EditorWindow();

//	Functions

public:

	/**
	* @brief Update window
	*/
	virtual void Update();

	/**
	* @brief Widget display function
	*/
	virtual void Display();


	/**
	* @brief Usually called at the beginning of each window display functions.
	* This function manage the creation of the window
	* WARNING : must place EndWidget at the end
	*/
	virtual bool BeginWindow();


	/**
	* @brief Usually called at the end of each window display functions.
	* WARNING : Must be placed when a BeginWidget has been placed
	*/
	virtual void EndWindow();


	/**
	* @brief Rename window
	*/
	void Rename(const std::string& newName);

	/**
	* @brief Get window name
	*/
	std::string GetName() const;
};
#pragma once

#include <memory>
#include <string>

#include "EditorTheme.hpp"

#define WIDGETNAME_CHAR_MAX 28

/**
@brief This is the parent class of all widgets, can be used if you want to create your own widgets
*/
class EditorWidget
{

//	Variables

protected:

	std::unique_ptr<struct ImGuiWindowClass> m_windowClass = nullptr;

	bool m_externViewport = false;
	int  m_flag = 0;
	std::string	 m_nameID;
	std::string  m_name = "widget";

	const std::string m_ID;

	bool m_keepFocus = false;

public:

	bool isActive = false;
	bool isVisible = false;

//	Constructor(s) & Destructor(s)

public:

	EditorWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive = false, const std::string& typeID = "###WIDGET");
	virtual ~EditorWidget();

//	Functions

public:

	/**
	@brief Update widget
	*/
	virtual void Update();

	/**
	@brief Widget display function
	*/
	virtual void Display();


	/**
	@brief Usually called at the beginning of each widget display functions.
	This function manage the creation of the window
	WARNING : must place EndWidget at the end

	@return Return true if the widget is open
	*/
	virtual bool BeginWidget();


	/**
	@brief Usually called at the end of each widget display functions.
	WARNING : Must be placed when a BeginWidget has been placed
	*/
	virtual void EndWidget();


	/**
	@brief Rename widget
	*/
	void Rename(const std::string& newName);

	/**
	* @brief Get widget name
	*/
	std::string GetName() const;
};
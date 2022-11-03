#pragma once

#include "Widgets/EditorWidget.hpp"

/**
* @brief This the toolbar which is usually placed at the top of the window
* It contains tool for the user
*/
class ToolbarWidget : public EditorWidget
{

//	Variables

private:

	unsigned int m_playButtonTexture;
	unsigned int m_pauseButtonTexture;
	unsigned int m_stopButtonTexture;
	unsigned int m_compileButtonTexture;

//	Constructor(s) & Destructor(s)

public:

	ToolbarWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive = false);

//	Functions

public:


	/**
	@brief Usually called at the beginning of each widget display functions.
	This function manage the creation of the window
	WARNING : must place EndWidget at the end

	@return Return true if the widget is open
	*/
	bool BeginWidget() override;

	/**
	* @brief Widget display function
	*/
	void Display() override;
};
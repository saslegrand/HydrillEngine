#pragma once

#include "Widgets/EditorWidget.hpp"

#include <string>
#include <vector>
#include <mutex>

#include <Types/Color.hpp>
#include <Tools/Event.hpp>

//	Forward declarations

enum class MessageType;

/**
@brief Console widget for editor
This show the console of the engine
and is linked with the debug class
*/
class ConsoleWidget : public EditorWidget
{

//	Internal classes definitions

private:

	struct ConsoleLine
	{
		std::string date;
		std::string text;
		Color4 color;
		int type;
	};

//	Variables

private:

	std::vector<ConsoleLine> m_lines;

	bool m_followingScrollMode = true;
	bool m_updated	= false;
	bool m_showDate = true;
	int  m_maskType = 0;

	unsigned int m_warningCount = 0;
	unsigned int m_errorCount = 0;

	unsigned int m_followingScrollModeTexture = 0;
	unsigned int m_clearTexture = 0;
	unsigned int m_showDateTexture = 0;
	unsigned int m_infoTexture = 0;
	unsigned int m_successTexture = 0;
	unsigned int m_warningTexture = 0;
	unsigned int m_errorTexture = 0;

	float m_elapsedTime = 0.0f;
	CallbackID m_printCallbackID;

	Color4 m_inactiveButtonColor = Color4(0.5f, 0.5f, 0.5f, 0.25f);
	Color4 m_defaultButtonColor = Color4(0.8f,0.8f,0.8f,1.f);

	Color4 m_infoButtonColor     = Color4::Blue;
	Color4 m_successButtonColor  = Color4::Green;
	Color4 m_errorButtonColor    = Color4::Red;
	Color4 m_warningButtonColor = Color4::DarkYellow;

//	Constructor(s) & Destructor(s)

public:

	ConsoleWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive = false);

	virtual ~ConsoleWidget();

//	Functions

private:

	/**
	@brief Funcion that need to subscribe to the logger print event from where messages will come from
	*/
	void LogPrint(const std::string& date, const std::string& line, const int type);

	/**
	@brief Manage the option of the console.
	Calls ImGui functions, so must be placed during between Imgui frame calls.
	*/
	void Options();

	/**
	@brief Content of the console, manage the displaying of the logs in the console.
	Calls ImGui functions, so must be placed during between Imgui frame calls.
	*/
	void Content();

	/**
	@brief Checkbox used to hide or show a type of message in the console.
	Calls ImGui functions, so must be placed during between Imgui frame calls.
	
	@param textureID : texture of the button
	@param type :  type of text to show/hide
	@param size : button size x and y
	@param activeColor : color when activated
	*/
	void MessageTypeButton(unsigned int textureID, const MessageType& type, float size, Color4 color);

public:

	/**
	@brief Widget updaye function
	*/
	void Update() override;

	/**
	@brief Widget display function
	*/
	void Display() override;

};
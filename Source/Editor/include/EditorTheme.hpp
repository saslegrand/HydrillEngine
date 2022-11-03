#pragma once

#include <memory>

#include "GUITypes.hpp"


//	Forward declaration

struct ImGuiIO;
struct ImGuiStyle;
struct ImVec4;

/**
* @brief This class is an instance which contain layered function of imgui
* Also stock datas like theme colors, fonts...
*/
class EditorTheme
{

//	Variables

private:

	ImVec4* m_styleColors;

public:

	std::unique_ptr<GUI::FontGroup> defaultFont;
	std::unique_ptr<GUI::FontGroup> consoleFont;

	GUI::ColorTheme color_borders;
	GUI::ColorTheme color_backgrounds;
	GUI::ColorTheme color_interactives;
	GUI::ColorTheme color_interactiveBackgrounds;
	GUI::ColorTheme color_buttons;

	Color4 police;
	Color4 disabledPolice;

	ImGuiStyle* style;
	ImGuiIO* IO;

	Color4 m_colorX;
	Color4 m_colorY;
	Color4 m_colorZ;
	Color4 m_colorW;

//	Constructors

public:

	EditorTheme();
	~EditorTheme();

//	Functions

public:

	/**
	* @brief Set the color of the enterred target index
	* @param target : Target of the color change
	* @param color : New color to apply
	*/
	void SetStyleColor(const int target, const Color4& color);

	/**
	* @brief Initialize color for editor theme
	*/
	void InitializeColors();

	/**
	* @brief Send colors to ImGui
	*/
	void SetColors();
};

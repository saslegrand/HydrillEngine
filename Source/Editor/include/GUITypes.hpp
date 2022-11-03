#pragma once
#include <Types/Color.hpp>

//	Forward declarations

struct ImGuiIO;
struct ImFont;

//	Enumerations

enum FontTypes
{
	FontTypes_Regular,
	FontTypes_Light,
	FontTypes_Bold,
	FontTypes_Italic,
	FontTypes_Thin
};

namespace GUI
{
	class FontGroup
	{

	//	Variables

	private:

		ImGuiIO* m_io = nullptr;

		ImFont* m_regular = nullptr;
		ImFont* m_light = nullptr;
		ImFont* m_bold = nullptr;
		ImFont* m_italic = nullptr;
		ImFont* m_thin = nullptr;

		float m_size  = 15;


	//	Constructors

	public:

		FontGroup() = default;
		FontGroup(const float size, ImGuiIO * io);

	//	Functions

	private:

		/**
		* @brief Add font and register it in the passed font
		* @param out_font : Font to load in
		* @param path : Font path
		* @param isdefault : Should be the default font ?
		*/
		void LoadFont(ImFont*& out_font, const char* path, const bool isdefault = false);

	public:

		/**
		* @brief Add font of the selected type
		* @param fontType : Type of the font to load in
		* @param path : Path of the new font
		* @param isdefault : Should be the default font ?
		*/
		void AddFont(const unsigned int fontType, const char* path, const bool isdefault = false);

		/**
		* @brief Push regular style font (Must call EndFontStyle())
		*/
		void UseRegular();

		/**
		* @brief Push light style font (Must call EndFontStyle())
		*/
		void UseLight();

		/**
		* @brief Push bold style font (Must call EndFontStyle())
		*/
		void UseBold();

		/**
		* @brief Push italic style font(Must call EndFontStyle())
		*/
		void UseItalic();

		/**
		* @brief Push thin style font(Must call EndFontStyle())
		*/
		void UseThin();

		/**
		* @brief Pop current font style (must have called a UseStyle function before)
		*/
		void EndFontStyle();
	};

	/**
	* Color4 display mode direction (COLORTHEME_LIGHT : lighter on hover and more on activation, COLORTHEME_DARK : darker on hover and more on activation)
	*/
	enum ColorThemeGen
	{
		COLORTHEME_LIGHT = 0,
		COLORTHEME_DARK = 1,
	};

	struct ColorTheme
	{
		//	Variables

		Color4 defaultColor;
		Color4 hoveredColor;
		Color4 activeColor;
		Color4 unfocusedInactiveColor;
		Color4 unfocusedActiveColor;

		unsigned int config = COLORTHEME_LIGHT;

		//	Constructors

		ColorTheme() = default;

		//	Functions

		/**
		* @brief Generate color theme with different variations
		*/
		void AutoGen();
	};


	class Dockspace
	{

	//	Variables

	private:

		bool m_initialized = false;
		const char* m_name;
		const char* m_dockID;

		int m_windowFlags = 0;
		int m_dockFlags = 0;

	//	Constructors

	public:

		/**
		* @brief Initialize the dockspace which is necessary to dock widget in our main window
		* @param const char* name = "DockWindow"
		* @param const char* dockID = "DockSpaceID_01"
		*/
		Dockspace(const char* name = "DockWindow", const char* dockID = "DockSpaceID_01");
	
	//	Functions

	public:

		/**
		* @brief Display a dockSpace which take all the main window size
		* (you must place EndDockSpace() at the end of the dockspace content)
		* @return true if the dockspace is open
		*/
		bool Begin();

		/**
		* @brief End dockcspace
		*/
		void End();
	};
}


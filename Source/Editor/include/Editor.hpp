#pragma once

#include <memory>

#include "EditorContext.hpp"

//	Forward Declaration class

class Window;
class EditorTheme;
class EditorImages;
class EditorResourcePreview;
class EditorSelectionManager;
class EditorDragManager;
class EditorData;
class FileDialogs;

class WidgetManager;
class WindowManager;
class DllWrapper;
class TaskQueue;

namespace GUI
{
	class Dockspace;
}

class Editor
{

//	Variables

private:

	EditorContext m_context;

	bool m_dockSpaceInitialized = false;

	int  m_dockspaceFlags = 0;
	int  m_dockspaceWindowFlags = 0;

	std::unique_ptr<GUI::Dockspace> m_mainDockSpace;

	//	Editor Context variables

	std::unique_ptr<EditorTheme>			 m_theme;
	std::unique_ptr<EditorImages>			 m_images;
	std::unique_ptr<EditorResourcePreview>	 m_resourcePreview;
	std::unique_ptr<EditorSelectionManager>  m_selectionManager;
	std::unique_ptr<EditorData>				 m_data;
	std::unique_ptr<EditorDragManager>		 m_dragManager;
	std::unique_ptr<FileDialogs>			 m_fileDialog;

	std::unique_ptr<WidgetManager>	m_widgetManager;
	std::unique_ptr<WindowManager>	m_windowManager;

	std::unique_ptr<TaskQueue>		m_taskQueue;

public:

	bool shutdown = false;
	bool running  = false;

//	Constructor(s) & Destructor(s)

public:

	Editor(Window* window);
	virtual ~Editor();

	
//	Functions

private:

	/**
	* @brief Display top bar of the dockspace (should be placed between BeginDockSpace() and EndDockSpace()
	*/
	void DisplayMenuBar();

public:

	/**
	* @brief Initialize function of interface. Create Imgui context, apply the default theme and initialized widgets
	*/
	void Initialize();

	/**
	* @brief Editor interface update function
	*/
	void Update();

	/**
	* @brief Editor interface display function
	*/
	void Display();

	/**
	* @brief Must be placed at the beginning of each frame
	*/
	void FrameStart();

	/**
	* @brief Must be placed at the end of each frame
	*/
	void Render();
};

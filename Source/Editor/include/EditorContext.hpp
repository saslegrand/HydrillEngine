#pragma once

#include <functional>
#include <queue>

#include "Tools/Event.hpp"
#include "EditorImages.hpp"
#include "Types/GUID.hpp"

class Resource;
class GameObject;
class EditorTheme;
class EditorImages;
class WidgetManager;
class WindowManager;
class EditorSelectionManager;
class EditorDragManager;
class EditorResourcePreview;
class EditorData;
class DllWrapper;
class TaskQueue;
class FileDialogs;
class Window;
class Camera;


enum class GameState
{
	Pause,
	Play,
	Sleep
};

/**
* @brief  Context of the editor, contains Theme of the editor
*/
struct EditorContext
{
	friend class Editor;

//	Variables

private:

	static EditorContext* instance;

public:
	WidgetManager* widgetManager = nullptr;
	WindowManager* windowManager = nullptr;

	EditorTheme*  theme = nullptr;
	EditorImages* images = nullptr;
	DllWrapper* scriptingWapper = nullptr;

	EditorResourcePreview*	resourcePreview;
	EditorDragManager*	    dragManager		 = nullptr;
	EditorSelectionManager* selectionManager = nullptr;
	EditorData*				editorDatas		 = nullptr;
	FileDialogs*			editorFileDialog = nullptr;

	Window* window = nullptr;

	HYGUID idLastActiveCamGO;

	GameState gameState = GameState::Sleep;

	Event<> onEnginePlayed;
	Event<> onEnginePaused;
	Event<> onEngineStopped;

	TaskQueue* taskQueue;

	bool allowGameObjectCopy;

//	Functions

public:

	static EditorContext& Instance();

	/**
	Return the application window width

	@return int : Window width
	*/
	int GetWindowWidth() const;

	/**
	Return the application window height

	@return int : Window height
	*/
	int GetWindowHeight() const;
};

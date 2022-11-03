#pragma once

#include <vector>
#include <memory>

#include "Widgets/ToolbarWidget.hpp"
#include "Widgets/ConsoleWidget.hpp"
#include "Widgets/GameObjectInspectorWidget.hpp"
#include "Widgets/ResourceInspectorWidget.hpp"
#include "Widgets/SceneGraphWidget.hpp"
#include "Widgets/ContentBrowserWidget.hpp"
#include "Widgets/RenderGameWidget.hpp"
#include "Widgets/RenderSceneWidget.hpp"
#include "Widgets/EditorWidget.hpp"

#define MAX_DUPLICATED_WIDGET 3

//	Forward Declaration class

namespace GUI
{
	class EditorTheme;
}

//	Enum

enum WidgetTypes
{
	WidgetTypes_Default,
	WidgetTypes_Console,
	WidgetTypes_Scene,
	WidgetTypes_Game,
	WidgetTypes_GameObjectInspector,
	WidgetTypes_ResourceInspector,
	WidgetTypes_ContentBrowser,
	WidgetTypes_SceneGraph,
	WidgetTypes_ToolBar,

	WidgetTypes_Count
};


/**
@brief This class Manage all widgets of the editor
*/
class WidgetManager
{

//	Variables

private:

	//	Unique Widgets
	std::unique_ptr<ToolbarWidget>		m_toolbar		= nullptr;
	std::unique_ptr<RenderGameWidget>	m_gameWindow	= nullptr;
	std::unique_ptr<RenderSceneWidget>	m_sceneWindow	= nullptr;
	std::unique_ptr<ConsoleWidget>		m_console		= nullptr;
	std::unique_ptr<SceneGraphWidget>	m_sceneGraph	= nullptr;

	//	Multiplicable widgets
	std::unique_ptr<GameObjectInspectorWidget>	m_gameObjectInspectors[MAX_DUPLICATED_WIDGET];
	std::unique_ptr<ResourceInspectorWidget>	m_resourceInspectors[MAX_DUPLICATED_WIDGET];
	std::unique_ptr<ContentBrowserWidget>		m_contentBrowsers[MAX_DUPLICATED_WIDGET];

	std::vector<EditorWidget*> m_widgetList;

//	Constructor(s) & Destructor(s)

public:

	WidgetManager();
	~WidgetManager();

//	Variables

public:
	/**
	@brief Return the render scene widget
	
	@return RenderSceneWidget& : Scene widget reference
	*/
	RenderSceneWidget& GetSceneWidget() const;

	/**
	@brief Add a widget, of the selected type (number of same widget is limited to 3)
	@param type : Widget type
	@param name : Widget name
	*/
	void AddWidget(const unsigned int type, const std::string& name);

	/**
	@brief Display all the widgets
	*/
	void DisplayWidgets();

	/**
	@brief Update all the widgets
	*/
	void UpdateWidgets();

	/**
	@brief menu bar to manage widgets
	*/
	void WidgetMenuBar();
};

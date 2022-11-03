#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <queue>

#include "Widgets/EditorWidget.hpp"

//	Forward declarations

class GameObject;

/**
* @brief Console widget for editor
* This show the console of the engine
* and is linked with the debug class
*/
class SceneGraphWidget : public EditorWidget
{

//	Variables

private:

	int m_startRangeIndex = -1;
	int m_endRangeIndex = -1;
	int m_count = 0;

	bool m_hasScene = false;
	bool m_contextMenuOpened = false;

	bool m_draggingObject = false;
	bool m_draggingSelected = false;
	bool m_tryRename = false;

	GameObject* m_pressedItem = nullptr;

	std::vector<const GameObject*> m_toForceOpen;

	std::vector<GameObject*> m_copied;


	GameObject* m_renamingObject = nullptr;
	std::string m_renameBuffer = "";

	std::queue<std::function<void()>> m_taskQueue;

//	Constructor(s) & Destructor(s)

public:

	SceneGraphWidget(const std::string& widgetName, const unsigned int widgetID, const bool isActive = false);

//	Functions

private:

	/**
	@brief Reset selected object to none, and clear multi-selected object list
	*/
	void ResetSelections();

	/**
	@brief Duplicate selected objects
	*/
	void Duplicate();

	/**
	@brief Copy selected objects
	*/
	void Copy(GameObject* gameObject = nullptr);

	/**
	@brief Paste copied objects
	*/
	void Paste(GameObject* parent = nullptr);

	/**
	@brief Instantiate a gameObject with a mesh

	@param name : gameObject name
	@param path : mesh resource path
	@param parent : optional parent
	*/
	GameObject* InstantiateMesh(const std::string& name, const std::string& path, GameObject* parent = nullptr);

	/**
	@brief Instantiate a gameObject from a prefab

	@param name : gameObject name
	@param path : prefab resource path
	@param parent : optional parent
	*/
	GameObject* InstantiatePrefab(const std::string& name, const std::string& path, GameObject* parent = nullptr);

public:

	/**
	@brief Display GameObject and its children if it has any
	 
	@param gameObject : recusrsion is used through this parameters, start by calling root object
	*/
	void DisplayGameObject(GameObject* gameObject);
	
	/**
	@brief Update all selected object while selecting multiple objects
	
	@param gameObject : recusrsion is used through this parameters, start by calling root object
	*/
	void UpdateMultiSelectedObject(GameObject* gameObject);

	/**
	@brief Drops from outside the scene graph

	@param parent : gameobject in which the drop happens into
	*/
	bool ExternalDrop(GameObject* parent);

	/**
	@brief Widget display function
	*/
	void Display() override;


	/**
	@brief Scene graph Widget display function, it update selected object list if maj was pressed.
	*/
	void Update() override;

};
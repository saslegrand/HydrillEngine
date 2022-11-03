#pragma once

#include <memory>
#include <vector>

#include <Types/GUID.hpp>
#include <Tools/Event.hpp>

#include "Maths/Vector3.hpp"
#include "EditorImages.hpp"

class Resource;
class GameObject;
class Camera;

/**
@brief Editor selection manager
*/
class EditorSelectionManager
{

//	Variables

private:

	std::vector<HYGUID>      m_gameObjects;
	std::vector<Resource*>	 m_resources;

public:

	Event<HYGUID*>     onGameObjectUpdated;
	Event<Resource*>   onResourceUpdated;

	Vector3 selectedObjectPosition = Vector3::Zero;
	Vector3 selectedObjectRotation = Vector3::Zero;
	Vector3 selectedObjectScale = Vector3::Zero;

//	Functions

public:

	/**
	@brief Add a GameObject to the selected gameObject list

	@param selectedGameObject : GameObject's HYGUID to add

	@return Was the adding a success ?
	*/
	bool AddToSelection(HYGUID& selectedGameObject);

	/**
	@brief Add a GameObject to the selected gameObject list

	@param selectedGameObject : GameObject to add

	@return Was the adding a success ?
	*/
	bool AddToSelection(const GameObject& selectedGameObject);

	/**
	@brief Add a Resource to the selected resource list

	@param selectedResource : Resource to add

	@return Was the adding a success ?
	*/
	bool AddToSelection(Resource*	selectedResource);

	/**
	@brief Remove a GameObject from the selected gameObject list

	@param selectedGameObject : GameObject's HYGUID to add

	@return Was the removing a success ?
	*/
	bool RemoveFromSelection(const HYGUID& selectedGameObject);

	/**
	@brief Remove a GameObject from the selected gameObject list

	@param selectedGameObject : GameObject to add

	@return Was the removing a success ?
	*/
	bool RemoveFromSelection(const GameObject& selectedGameObject);

	/**
	@brief Remove a Resource from the selected resource list

	@param selectedResource : Resource to add

	@return Was the removing a success ?
	*/
	bool RemoveFromSelection(Resource* selectedResource);

	/**
	@brief Delete selected game objects
	*/
	void DeleteGameObjects();

	GameObject* GetGameObject(size_t index);

	GameObject* GetGameObjectFromScene(const HYGUID& guid);

	/**
	@brief Get all selected gameObjects

	@return list of selected GameObject's HYGUID
	*/
	std::vector<HYGUID>& GetGameObjects();

	/**
	@brief Get all selected resources

	@return list of selected Resources
	*/
	std::vector<Resource*>&	  GetResources();

	/**
	@brief Is the given GameObject currently selected

	@param gameObject : GameObject's HYGUID to check if selected or not 

	@return true if selected
	*/
	bool IsSelected(const HYGUID& gameObject);

	/**
	@brief Is the given GameObject currently selected

	@param gameObject : GameObject to check if selected or not

	@return true if selected
	*/
	bool IsSelected(const GameObject& gameObject);

	/**
	@brief Is the given Resource currently selected

	@param resource : Resource to check if selected or not

	@return true if selected
	*/
	bool IsSelected(Resource* resource);

	/**
	@brief Clear all selected items
	*/
	void ClearAll();

	/**
	@brief Clear all selected GameObject
	*/
	void ClearGameObjects();

	/**
	@brief Clear all selected Resource
	*/
	void ClearResources();
};

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include "EngineContext.hpp"
#include "Core/Logger.hpp"
#include "ECS/Component.hpp"
#include "ECS/Transform.hpp"
#include "ECS/Systems/SystemManager.hpp"

#include "Generated/GameObject.rfkh.h"

/**
@brief Base game entity
*/
class HY_CLASS() GameObject : public SceneObject
{
private:
	//std::unordered_map<HYGUID, int> m_compLink;
	std::vector<Component*> m_components;

	GameObject* m_parent = nullptr;
	std::vector<GameObject*> m_children;

#pragma warning(disable:4251) // Warning disabled because std::string of this DLL could conflict with the std::string of the project it is used by.
	std::string m_tag = "Default";
#pragma warning(default:4251) // But only our Editor & Game projects will use it, so it will be fine


public:
	Transform transform;
	bool isStatic = false;
	bool m_isDestroyed = false;
	bool m_parentActive = true;

public:
	/**
	@brief Default constructor
	*
	@param name : Name of the GameObject
	*/
	GameObject(const std::string & name, const HYGUID & uid = HYGUID::NewGUID());

private:
	/**
	@brief Check if the gameobject has a specific child

	@param GO : Child to check

	@return bool : true if GO possesed, false otherwise
	*/
	bool HasChild(GameObject & GO) const;

	/**
	@brief Check if the gameobject has a direct parent
	
	@return bool : true if has direct parent, false otherwise
	*/
	bool HasDirectParent() const;

	/**
	@brief Check if the gameobject is at the root node
	
	@return bool : true if at the root node, false otherwise
	*/
	bool IsAtTheRootNode() const;

	/**
	@brief Add a child to the gameobject
	
	@param GO : Gameobject to add
	*/
	void AddChild(GameObject & GO);

	/**
	@brief Remove a child to the gameobject
	
	@param GO : Gameobject to remove
	*/
	void RemoveChild(GameObject & GO);

	/**
	@brief Update the gameobject and children transforms
	*/
	void ComputeSelfAndChildren();

	/**
	* @brief Remove the gameobject's from root note (Call only on destroy)
	*/
	void DetachFromRoot();

	/*
	Erase the specified component and call OnDestroy
	*/
	void InternalDestroyComponent(Component& component, bool shouldCallOnDestroy);

	/**
	Activate/Deactivate internal for the gameobject

	This will also deactivate children and components
	*/
	ENGINE_API void SetActiveInternal();

public:
	/**
	Activate/Deactivate the gameobject

	@param isActive : activate if true, deactivate otherwise
	*/
	ENGINE_API bool IsActive() const;

	/**
	Tell if the gameobject is currently activated

	@return bool : True if activated, false otherwise
	*/
	ENGINE_API void SetActive(bool isActive) override;

	/**
	Tell if the gameobject is currently waiting for being destroyed
	*/
	bool IsDestroyed() const;

	/**
	Set the gameobject as destroyed (avoid multiple remove task)
	*/
	void SetAsDestroyed();

	/**
	@brief Function call when the GO is destroyed, Detach from scene
	*/
	ENGINE_API void OnDestroy(bool shoulCallOnDestroy);

	/**
	@brief Check if the given gameobject is a parent of this gameobject

	@param GO : Potential parent(direct/indirect) of the GameObject
	*/
	ENGINE_API bool IsDescendingFrom(GameObject * GO) const;

	/**
	@brief Check if the game object has a given tag

	@param tag : Tag to check

	@return bool : True if tags are equal, false otherwise
	*/
	bool HasTag(std::string_view tag);

	/**
	@brief Clear the gameobject from all references. Should only be used to clean a potentially broken gameObject.
	*/
	void Clear();

	/**
	@brief Update the gameobject's transform and children's transform if dirty
	*/
	void UpdateTransforms();

	/**
	@brief Set the gameobject's parent
	
	@param GO : New parent, if nullptr it removes the parent
	*/
	ENGINE_API void SetParent(GameObject* GO);

	/**
	* @brief Remove the gameobject's parent (return to scene root)
	*/
	ENGINE_API void DetachFromParent();

	/**
	@brief get the gameobject's children count
	
	@return int : Gameobject's children count
	*/
	int GetChildCount() const;

	/**
	@brief Return the gameobject's parent
	
	@return GameObject* : GameObject's parent, nullptr if at the scene root
	*/
	GameObject* GetParent() const;

	/**
	@brief Get the gameobject's child at index
	
	@return GameObject* : GameObject's child, nullptr if index out of bounds
	*/
	GameObject* GetChild(unsigned int childIndex) const;

	/**
	@brief Get the gameobject's children
	
	@return vector<GameObject*> : GameObject's children
	*/
	const std::vector<GameObject*>& GetChildren() const;

	std::vector<Component*> GetRawComponents() const;

	/**
	@brief Add a reflected component from it's name
	
	@param compName : Name of the component to add
	
	@return Component* : Return the added component or nullptr if fail
	*/
	ENGINE_API Component* AddComponentFromName(const std::string& compName, const HYGUID& compID = HYGUID::NewGUID());

	/**
	@brief Remove a component from it's gameObject

	@param component : Component to remove
	*/
	ENGINE_API static void DestroyComponent(Component& component);

	// TODO : Destroy with gameObject
	// Check for component garbage collector (component must not be destroyed on the system's loop)

	/**
	@brief Check if the gameObject has a specific component
	
	@return bool : true if component possesed, false otherwise
	*/
	template <DerivedComponent Comp>
	bool HasComponent();

	/**
	@brief Check if the gameObject has a specific component and returns it

	@param component : Pointer to the specific component (if found)

	@return bool : true if component found, false otherwise
	*/
	template <DerivedComponent Comp>
	bool TryGetComponent(Comp** component);

	/**
	@brief Return a specific owns component, you need to be sure that the GO has the component

	@return Comp* : Component found
	*/
	template <DerivedComponent Comp>
	Comp* GetComponent();

	/**
	@brief Get component by GUID

	@return Component* : Component found
	*/
	Component* GetComponent(const HYGUID& uid);

	/**
	@brief Return all owned components of a type, you need to be sure that the GO has the component

	@return vector<Comp*> : Components found
	*/
	template <DerivedComponent Comp>
	std::vector<Comp*> GetComponents();

	/**
	@brief Add a component to the GameObject. If the component is already added, check if it's unique
	
	@param args : Component constructor parameters
	
	@return Comp* : Component added and/or found (unique)
	*/
	template <DerivedComponent Comp>
	Comp* AddComponent();

	GameObject_GENERATED
};

#include "ECS/GameObject.inl"

File_GameObject_GENERATED
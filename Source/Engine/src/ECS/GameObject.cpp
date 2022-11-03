#include <Refureku/Refureku.h>

#include "EngineContext.hpp"

#include "Core/Logger.hpp"
#include "ECS/Behavior.hpp"
#include "Core/TaskQueue.hpp"
#include "Resources/SceneManager.hpp"

#include "Generated/GameObject.rfks.h"

GameObject::GameObject(const std::string& name, const HYGUID& uid)
	: SceneObject(name, uid), transform(*this)
{

}

void GameObject::UpdateTransforms()
{
	// Update all self and children if transform dirty
	if (transform.IsDirty())
	{
		ComputeSelfAndChildren();
		return;
	}
	
	for (auto& child : m_children)
		child->UpdateTransforms();
}

void GameObject::ComputeSelfAndChildren()
{
	// Update with parent global matrix if not at the root node
	if (!IsAtTheRootNode())
		transform.ComputeModelMatrices(m_parent->transform.GetWorldMatrix());
	else
		transform.ComputeModelMatrices();

	// Same for children
	for (auto& child : m_children)
		child->ComputeSelfAndChildren();
}

bool GameObject::IsActive() const
{
	return m_isActive && m_parentActive;
}

void GameObject::SetActiveInternal()
{
	GameObject* parent = GetParent();

	m_parentActive = parent == nullptr ? true : parent->IsActive();

	for (GameObject* child : m_children)
		child->SetActiveInternal();

	for (Component* child : m_components)
		child->OnModify();
}

void GameObject::SetActive(bool isActive)
{
	SceneObject::SetActive(isActive);

	SetActiveInternal();
}

#pragma region PARENT
void GameObject::SetParent(GameObject* GO)
{
	// Interpret SetParent(nullptr) as RemoveParent
	if (GO == nullptr)
	{
		DetachFromParent();
		return;
	}

	// Can't parent itself 
	if (GO == this || GO->IsDescendingFrom(this))
		return;

	// Remove current GO from parent
	if (m_parent != nullptr)
		m_parent->RemoveChild(*this);

	// Set new parent his child
	m_parent = GO;
	m_parent->AddChild(*this);

	transform.SetDirty(EDirtyFlags::World);
}

void GameObject::DetachFromParent()
{
	if (IsAtTheRootNode())
		return;

	transform.SetDirty(EDirtyFlags::World);

	// Set root node as parent
	SetParent(&EngineContext::Instance().sceneManager->GetCurrentScene()->GetRoot());
}

void GameObject::DetachFromRoot()
{
	if (!IsAtTheRootNode())
		return;

	m_parent->RemoveChild(*this);
}

void GameObject::InternalDestroyComponent(Component& comp, bool shouldCallOnDestroy)
{
	auto it = std::find(m_components.begin(), m_components.end(), &comp);

	if (it != m_components.end())
	{
		// Call OnDestroy
		Component* _comp = (*it);

		if (shouldCallOnDestroy)
			_comp->OnDestroy();

		// Erase from gameobject
		EngineContext::Instance().systemManager->DestroyComponent(*_comp);
		m_components.erase(it);
		return;
	}

	Logger::Error("GameObject - You try to remove a " + std::string(comp.getArchetype().getName()) + " component that does not exist");
}

void GameObject::OnDestroy(bool shoulCallOnDestroy)
{
	// Detach from parent
	DetachFromParent();

	// Detach from root node
	DetachFromRoot();

	// Erase components
	while (!m_components.empty())
		InternalDestroyComponent(*m_components[0], shoulCallOnDestroy);
}

#pragma endregion

#pragma region CHILDREN

void GameObject::RemoveChild(GameObject& GO)
{
	for (int i = 0; i < m_children.size(); ++i)
	{
		// Check if the child is possessed
		if (m_children[i] == &GO)
		{
			m_children.erase(m_children.begin() + i);
			return;
		}
	}
}

bool GameObject::IsDescendingFrom(GameObject* GO) const
{
	if (HasDirectParent())
	{
		if (m_parent == GO) return true;

		return m_parent->IsDescendingFrom(GO);
	}

	return false;
}

#pragma endregion

void GameObject::DestroyComponent(Component& component)
{
	component.SetActive(false);

	EngineContext::Instance().taskQueue->AddTask([&component]() {

		if (component.IsDestroyed())
			return;

		component.SetAsDestroyed();
		component.gameObject.InternalDestroyComponent(component, true);
		});
}

// Add a component to a gameobject from its class name. 
// The archetype is retrieved through the reflection database.
// Like that, we are able for example to create user behaviors out of the engine but keep the flowcontrol of them.
Component* GameObject::AddComponentFromName(const std::string& compName, const HYGUID& compID)
{
	// Check if the name is linked to an existing archetype (if the class is reflected)
	rfk::Class const* compClass = rfk::getDatabase().getFileLevelClassByName(compName.c_str());
	if (compClass)
	{
		// Check if the archetype is a sub class of Component
		if (!compClass->isSubclassOf(Component::staticGetArchetype()))
		{
			Logger::Error("GameObject - " + compName + " is not derived from component. Impossible to add " + compName + " class to a GameObject");
			return nullptr;
		}

		// Check if a behavior with the same archetype is already registered
		for (Component* comp : m_components)
		{
			if (comp->getArchetype() == *compClass)
			{
				// Check if the behavior registered can be instantiated multiple times
				if (comp->IsUnique())
				{
					Logger::Warning("GameObject - Behavior " + compName + " is unique and already added");
					return nullptr;
				}

				break;
			}
		}

		// Create the new component from its archetype in the system manager
		Component* newComp = EngineContext::Instance().systemManager->CreateComponent(*compClass, *this, compID);

		// Leave if creation failed
		if (newComp == nullptr)
		{
			Logger::Error("GameObject - Fail to add component '" + std::string(compClass->getName()) + "'");
			return nullptr;
		}

		// Add the component to the gameobject
		m_components.emplace_back(newComp);

		return newComp;
	}

	Logger::Error("GameObject - The class " + compName + " does not exist or is not reflected");
	return nullptr;
}

void GameObject::Clear()
{
	m_components.clear();
	m_children.clear();

	m_parent = nullptr;
}
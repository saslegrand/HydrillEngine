
inline bool GameObject::IsDestroyed() const
{
	return m_isDestroyed;
}

inline void GameObject::SetAsDestroyed()
{
	m_isDestroyed = true;

	SetActive(false);

	// Set components as destroyed
	for (Component* comp : m_components)
	{
		comp->SetAsDestroyed();
		comp->SetActive(false);
	}

	for (GameObject* child : m_children)
		child->SetAsDestroyed();
}

inline const std::vector<GameObject*>& GameObject::GetChildren() const
{
	return m_children;
}

inline Component* GameObject::GetComponent(const HYGUID& uid)
{
	auto it = std::find_if(m_components.begin(), m_components.end(), [&uid](Component* _comp) {
		return _comp->GetUID() == uid;
		});

	if (it != m_components.end())
		return *it;

	Logger::Warning("GameObejct - Can't find component with UID : " + std::string(uid) + " in GameObject " + std::string(m_name.data()));
	return nullptr;
}

inline std::vector<Component*> GameObject::GetRawComponents() const
{
	return m_components;
}

inline bool GameObject::HasTag(std::string_view tag)
{
	return m_tag.compare(tag) == 0;
}

inline bool GameObject::HasDirectParent() const
{
	return m_parent != nullptr;
}

inline bool GameObject::HasChild(GameObject& GO) const
{
	for (GameObject* child : m_children)
	{
		// Check if the child is possessed
		if (child == &GO)
			return true;
	}

	return false;
}

inline bool GameObject::IsAtTheRootNode() const
{
	return HasDirectParent() && !m_parent->HasDirectParent();
}

inline GameObject* GameObject::GetParent() const
{
	return !IsAtTheRootNode() ? m_parent : nullptr;
}

inline void GameObject::AddChild(GameObject& GO)
{
	m_children.emplace_back(&GO);
}

inline int GameObject::GetChildCount() const
{
	return static_cast<int>(m_children.size());
}

inline GameObject* GameObject::GetChild(unsigned int childIndex) const
{
	return childIndex < m_children.size() ? m_children[childIndex] : nullptr;
}

template <DerivedComponent Comp>
inline bool GameObject::HasComponent()
{
	for (Component* comp : m_components)
	{
		if (Comp::staticGetArchetype() == comp->getArchetype())
			return true;
	}

	return false;
}

template <DerivedComponent Comp>
inline bool GameObject::TryGetComponent(Comp** component)
{
	for (Component* comp : m_components)
	{
		if (Comp::staticGetArchetype() == comp->getArchetype())
		{
			*component = static_cast<Comp*>(comp);
			return true;
		}
	}

	return false;
}

template <DerivedComponent Comp>
inline Comp* GameObject::GetComponent()
{
	Comp* comp = nullptr;
	if (TryGetComponent(&comp))
		return comp;

	Logger::Error("GameObject - Component not exists");

	return nullptr;
}

template <DerivedComponent Comp>
inline std::vector<Comp*> GameObject::GetComponents()
{
	std::vector<Comp*> comps;
	for (Component* comp : m_components)
	{
		if (Comp::staticGetArchetype() == comp->getArchetype())
			comps.push_back(static_cast<Comp*>(&comp));
	}

	// No component found
	if (comps.size() == 0)
		Logger::Error("GameObject - Component not exists");

	return comps;
}

// Base function to add a component to a gameobject
template <DerivedComponent Comp>
inline Comp* GameObject::AddComponent()
{
	Comp* comp = nullptr;

	// Look if the component is already added to the game object
	if (TryGetComponent(&comp))
	{
		// If already added, check if it can be added multiple times
		if (comp->IsUnique())
		{
			Logger::Warning("GameObject - Component " + std::string(Comp::staticGetArchetype().getName()) + " is unique and already added");
			return comp;
		}
	}

	// Create an unique identifier and retrieve the component archetype
	HYGUID compID = HYGUID::NewGUID();
	rfk::Class const& compClass = Comp::staticGetArchetype();

	// Create the new component from its archetype in the system manager
	Component* newComp = EngineContext::Instance().systemManager->CreateComponent(compClass, *this, compID);

	// Leave if creation failed
	if (newComp == nullptr)
	{
		Logger::Error("GameObject - Fail to add component '" + std::string(Comp::staticGetArchetype().getName()) + "'");
		return nullptr;
	}

	// Add the component to the gameobject
	m_components.emplace_back(newComp);

	return static_cast<Comp*>(newComp);
}

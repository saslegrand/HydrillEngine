#include "ECS/Systems/BehaviorSystem.hpp"

#include "ECS/Behavior.hpp"
#include "Core/Logger.hpp"

#include "EngineContext.hpp"
#include "Core/TaskQueue.hpp"

void BehaviorSystem::StartSystem()
{
	m_shouldInitiateBehavior = true;

	AwakeAll();
	StartAll();
}

void BehaviorSystem::StopSystem()
{
	m_shouldInitiateBehavior = false;
	m_startInitialize.ClearCallbacks();
	m_awakeInitialize.ClearCallbacks();
}

void BehaviorSystem::InitializeNewBehaviors()
{
	if (!m_addBehavior)
		return;

	m_addBehavior = false;

	// Call Awake on behaviors added in game
	m_awakeInitialize.Invoke();
	m_awakeInitialize.ClearCallbacks();

	// Call Start on behaviors added in game
	m_startInitialize.Invoke();
	m_startInitialize.ClearCallbacks();
}

void BehaviorSystem::AwakeAll()
{
	const size_t size = m_behaviors.size();
	for (size_t i = 0; i < size; i++)
	{
		Behavior& behavior = *m_behaviors[i];
		if (behavior.IsActive())
			behavior.Awake();
	}
}

void BehaviorSystem::StartAll()
{
	const size_t size = m_behaviors.size();
	for (size_t i = 0; i < size; i++)
	{
		Behavior& behavior = *m_behaviors[i];
		if (behavior.IsActive())
			behavior.Start();
	}
}

void BehaviorSystem::UpdateAll(float tick)
{
	// Initialize new behaviors added in game
	InitializeNewBehaviors();

	const size_t size = m_behaviors.size();
	for (size_t i = 0; i < size; i++)
	{
		Behavior& behavior = *m_behaviors[i];
		if (behavior.IsActive())
			behavior.Update(tick);
	}
}

void BehaviorSystem::FixedUpdateAll(float fixedTick)
{
	const size_t size = m_behaviors.size();
	for (size_t i = 0; i < size; i++)
	{
		Behavior& behavior = *m_behaviors[i];
		if (behavior.IsActive())
			behavior.FixedUpdate(fixedTick);
	}
}

void BehaviorSystem::LateUpdateAll()
{
	const size_t size = m_behaviors.size();
	for (size_t i = 0; i < size; i++)
	{
		Behavior& behavior = *m_behaviors[i];
		if (behavior.IsActive())
			behavior.LateUpdate();
	}
}

// Base function to create a Behavior instance in the system.
// The behavior is fully drived by the system and reference to its owner for more control from the user.
Behavior* BehaviorSystem::AddBehaviorInstance(rfk::Class const& archetype, GameObject& owner, const HYGUID& id)
{
	// Add a behavior to the system. 
	// The behavior is created from its archetype to make use of the dynamic reflection and the polymorphism
	Behavior& behavior = *m_behaviors.emplace_back(archetype.makeUniqueInstance<Behavior>(owner, id));

	if (m_shouldInitiateBehavior)
	{
		m_addBehavior = true;
		m_awakeInitialize.AddCallback(&Behavior::Awake, behavior);
		m_startInitialize.AddCallback(&Behavior::Start, behavior);
	}

	return &behavior;
}

void BehaviorSystem::RemoveBehaviorInstance(Behavior& behavior)
{
	// Find the the behavior
	auto it = std::find_if(m_behaviors.begin(), m_behaviors.end(), 
		[&behavior](std::unique_ptr<Behavior>& _behavior) { return _behavior.get() == &behavior; });

	if (it == m_behaviors.end())
	{
		// Should not happer
		Logger::Error("BehaviorSystem - The behavior you tried to remove not exists");
		return;
	}

	// Remove the behavior
	m_behaviors.erase(it);
}
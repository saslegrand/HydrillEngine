#include "ECS/GameObject.hpp"

#include "Resources/SceneManager.hpp"
#include "IO/Keyboard.hpp"
#include "IO/Mouse.hpp"

#include "Generated/MyScript.rfks.h"

rfk::UniquePtr<MyScript> MyScript::defaultInstantiatorID(GameObject& gameObject, const HYGUID& uid)
{
	return rfk::makeUnique<MyScript>(gameObject, uid);
}

MyScript::MyScript(GameObject& gameObject, const HYGUID& uid)
	: Behavior(gameObject, getArchetype().getName(), uid)
{
	// Tell if the component should be unique to the gameObject (1 per owner)
	m_isUnique = false;
}

void MyScript::MyModifyMethod()
{
	// Call when the Vector3 is modified
	Logger::Info("Modifying my vector3. Do something here");
}

void MyScript::Awake()
{
	// Do something here
}

void MyScript::Start()
{
	// Do something here
}

void MyScript::Update(float tick)
{
	// Do something here (tick = current deltaTime)
	return;

	// Inputs
	Keyboard::GetKeyPressed(Key::Enter);
	Keyboard::GetKeyPressed("MyCustomKey");

	float axisValue = Keyboard::GetAxisValue("MyAxisName");

	bool isClicked = Mouse::GetButtonClicked(MouseButton::Middle);



	// Destroy a gameobject
	SceneManager::DestroyGameObject(gameObject);

	// Load a new Scene
	SceneManager::LoadNewCurrentScene("MySceneName");

	// Retrieve a gameobject from its name
	SceneManager::FindGameObjectWithName("GOName");



	// Destroy a component
	GameObject::DestroyComponent(*gameObject.GetComponent<Component>());



	// Add a colision event
	collider = gameObject.GetComponent<SphereCollider>();
	CallbackID id = collider->OnCollisionEnter.AddCallback(&MyScript::CollisionCallback, *this);



	// Normal log
	Logger::Info("Info");
	// Warning log
	Logger::Warning("Warning");
	// Error log
	Logger::Error("Error");

}

void MyScript::CollisionCallback(CollisionInfo collision)
{
	// Collision happen (enter/exit/stay)
}

void MyScript::FixedUpdate(float fixedTick)
{
	// Do something here (might be related to a physic component)
}

void MyScript::LateUpdate()
{
	// Do something here
}

void MyScript::OnEnable()
{
	// Call when the component is Activate
}

void MyScript::OnDisable()
{
	// Call when the component is Deactivate
}

void MyScript::OnDestroy()
{
	// Call when the component is Destroy (only for game purpose)
	Logger::Info("Destroy script");
}
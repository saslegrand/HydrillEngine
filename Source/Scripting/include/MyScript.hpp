#pragma once

#include "Hydrill/MathsCollection.hpp"
#include "Hydrill/EngineCollection.hpp"
#include "Hydrill/ScriptingCollection.hpp"

#include "Resources/Resource/Mesh.hpp"
#include "Resources/Resource/Scene.hpp"
#include "ECS/Physics/SphereCollider.hpp"
#include "ECS/Sound/SoundEmitter.hpp"

#include "Generated/MyScript.rfkh.h"

class HY_CLASS() SCRIPTING_API MyScript final : public Behavior
{
private:
	// This a the default instantiator for the reflection module (Remove this will lead to a crash)
	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<MyScript> defaultInstantiatorID(GameObject& gameObject, const HYGUID& uid);
public:
	// Default constructor of the script. It should always be the last constructor to be called.
	MyScript(GameObject& gameObject, const HYGUID& uid);

	// Internal float not reflected. Only for the class purpose.
	float time = 0.f;

	SphereCollider* collider = nullptr;

	// Float that can be freely set. Header will help to organize fields
	HY_FIELD(PHeader("MyHeader"))
		float normalFloat = 2.f;

	// Int visible but not modifiable
	HY_FIELD(PReadOnly())
		int readInt = 666;

	// Float ranged between -5.f and 5.f
	HY_FIELD(PHeader("OneMoreHeader"), PRange(-5.f, 5.f))
		float rangedFloat = 0.f;

	// Vector2 hide. Not visible in inspector
	HY_FIELD(PHide())
		Vector2 hideVector2 = Vector2::Zero;

	// Vector3 wich will call a reflected given function if modified
	HY_FIELD(POnModify("MyModifyMethod"))
		Vector3 modifyVector3 = Vector3::Zero;

	// Vector of vector2. You can add, remove or swap elements.
	HY_FIELD()
		HyVector<Vector2> myVector;

	// Reference to a mesh resource, but not visible in the editor (only for save purpose)
	HY_FIELD(PHide())
		Mesh* mesh = nullptr;

	// Reference to a scene gameObject
	HY_FIELD()
		GameObject* go = nullptr;

	// Method call when the vector3 "modifyVector3" is modified. (Don't forget the 'HY_METHOD' macro)
	HY_METHOD()
		void MyModifyMethod();


	// -------------Behavior main methods -> Code the enity behavior here---------------

	// Awake function called when the game start or when the object is created in-game
	virtual void Awake() override;
	// Start function called after all the behaviors Awake or when the object is created in-game
	virtual void Start() override;
	// Update function call every frame
	virtual void Update(float tick) override;
	// FixedUpdate function call at a regular framerate
	virtual void FixedUpdate(float fixedTick) override;
	// LateUpdate function call every frame at th end of the update loop
	virtual void LateUpdate() override;
	// OnEnable call when the component is activate
	virtual void OnEnable() override;
	// OnDisable call when the component is deactivate
	virtual void OnDisable() override;
	// OnDestroy call when the component is destroyed
	virtual void OnDestroy() override;

	void CollisionCallback(CollisionInfo collision);

	MyScript_GENERATED
};

File_MyScript_GENERATED

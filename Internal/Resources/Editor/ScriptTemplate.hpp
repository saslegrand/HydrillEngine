#pragma once

// Main collection includes
// ------------------------
#include "Hydrill/MathsCollection.hpp"
#include "Hydrill/EngineCollection.hpp"
#include "Hydrill/ScriptingCollection.hpp"

// Add other include here
// ----------------------

// Script generated header file
// This MUST be the last include, other include symbols are needed
// ---------------------------------------------------------------
#include "Generated/$$SCRIPT_NAME$$.rfkh.h"

// Here is your main scripting class
// Write your behavior code inside and add it to your GameObjects
// --------------------------------------------------------------
class HY_CLASS() SCRIPTING_API $$SCRIPT_NAME$$ final : public Behavior
{
private:
	// $$SCRIPT_NAME$$ default instantiator metadata for reflection
	// This method MUST NOT be deleted
	// -------------------------------
	HY_METHOD(rfk::Instantiator)
	static rfk::UniquePtr<$$SCRIPT_NAME$$> defaultInstantiatorID(GameObject& gameObject, const HYGUID& uid);
public:
	// $$SCRIPT_NAME$$ default constructor
	// This method MUST NOT be deleted
	// -------------------------------
	$$SCRIPT_NAME$$(GameObject& gameObject, const HYGUID& uid);

	// Start function, do things when instancing your script
	// -----------------------------------------------------
	void Start() override;

	// Update function, call every frame
	// ---------------------------------
	void Update(float tick) override;

	// Class reflection macro
	// This macro MUST NOT be deleted
	// ------------------------------
	$$SCRIPT_NAME$$_GENERATED
};

// File reflection macro
// This macro MUST NOT be deleted
// ------------------------------
File_$$SCRIPT_NAME$$_GENERATED

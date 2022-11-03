#pragma once

#include "Resources/Resource/Resource.hpp"

#include "Generated/Script.rfkh.h"

/**
@brief Texture 
*/
class HY_CLASS() Script : public Resource
{
private:
	HY_METHOD(rfk::Instantiator)
		static rfk::UniquePtr<Script> defaultInstantiator(const HYGUID & uid);

public:

	ENGINE_API Script(const HYGUID& uid);

	bool Import(const std::string & path) override;
	void Create();

	virtual void Serialize() override;

	Script_GENERATED
};

File_Script_GENERATED

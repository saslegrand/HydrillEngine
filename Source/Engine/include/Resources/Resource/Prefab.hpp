#pragma once

#include <nlohmann/json.hpp>

#include "Resources/Resource/Resource.hpp"
#include "ECS/GameObject.hpp"

#include "Generated/Prefab.rfkh.h"

/**
@brief Prefab is a gameObject SceneGraph ready to be instantiated
*/
class HY_CLASS() Prefab : public Resource
{
private:
	HY_METHOD(rfk::Instantiator)
		static rfk::UniquePtr<Prefab> defaultInstantiator(const HYGUID & uid);

protected:
	json m_jsonGraph;

public:
	ENGINE_API Prefab(const HYGUID& uid);
	ENGINE_API Prefab(const HYGUID& uid, const std::string& name);

	/**
	@brief Create a prefab

	@param GameObject* : gameObject root
	*/
	ENGINE_API void CreateFromGameObject(GameObject* gameObject, const std::string& name, const std::string& path);

	/**
	@brief Get the json representing the prefab scene graph

	@return json& : json graph
	*/
	json& GetJsonGraph();

	virtual void Serialize() override;
	virtual void Deserialize() override;

	Prefab_GENERATED
};

File_Prefab_GENERATED
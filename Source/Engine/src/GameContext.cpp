#include "GameContext.hpp"

#include <nlohmann/json.hpp>

#include "Engine.hpp"
#include "Tools/PathConfig.hpp"
#include "Tools/SerializationUtils.hpp"

//	Context definition
GameContext* GameContext::m_instance = nullptr;

void GameContext::Render()
{
	m_instance->m_engine->GameRender();
}

void GameContext::Start()
{
	m_instance->m_engine->GameStart();
}

void GameContext::Stop()
{
	m_instance->m_engine->GameStop();
}

void GameContext::SaveSettings(std::string_view buildPath, std::string_view startScene)
{
	json jsonFile;

	// Save data
	json& jsonBuild = jsonFile["BuildData"];
	jsonBuild["StartScene"] = startScene.data();

	Keyboard::SaveKeys(jsonFile);

	GraphicsSettings& graphics = SystemManager::GetRenderSystem().graphicsSettings;
	graphics.Save(jsonFile);

	m_instance->WriteJson(buildPath.data(), jsonFile);
}

bool GameContext::LoadSettings()
{
	std::string settingsPath = INTERNAL_ENGINE_RESOURCES_ROOT + std::string("GameSettings.settings");
	if (!std::filesystem::exists(settingsPath))
	{
		Logger::Error("GameContext - Game settings file not exists, close application");
		return false;
	}

	json jsonFile;
	m_instance->ReadJson(settingsPath, jsonFile);

	Keyboard::LoadKeys(jsonFile);

	EngineContext& context = EngineContext::Instance();

	GraphicsSettings& graphics = SystemManager::GetRenderSystem().graphicsSettings;
	graphics.Load(jsonFile);

	// Save data
	std::string startSceneID = "";
	Serialization::TryGetValue(jsonFile["BuildData"], "StartScene", startSceneID);

	Scene* startScene = context.resourcesManager->GetResource<Scene>(HYGUID(startSceneID));
	if (startScene == nullptr)
	{
		Logger::Error("GameContext - Start scene is not valid, close application");
		return false;
	}

	context.sceneManager->ChangeCurrentScene(startScene);

	if (context.sceneManager->GetCurrentScene() == nullptr)
	{
		Logger::Error("GameContext - Scene '" + startScene->GetName() + "' is not registered, close application");
		return false;
	}

	return true;
}


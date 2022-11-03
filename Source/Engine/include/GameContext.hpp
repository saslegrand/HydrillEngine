#pragma once

#include "Types/Serializable.hpp"

/**
@brief This class allow use main loop functions of the engine (only for game purpose)
*/
class GameContext : public Serializable
{
	friend class Engine;

private:
	static GameContext* m_instance;
	class Engine* m_engine = nullptr;

public:
	/**
	@brief Call the engine game render pass
	*/
	static ENGINE_API void Render();
	
	/**
	@brief Call the engine game start
	*/
	static ENGINE_API void Start();

	/**
	@brief Call the engine game stop
	*/
	static ENGINE_API void Stop();

	/**
	@brief Save game settings in a file
	*/
	static ENGINE_API void SaveSettings(std::string_view buildPath, std::string_view startScene);

	/**
	@brief Load game settings from a file
	*/
	static ENGINE_API bool LoadSettings();
};

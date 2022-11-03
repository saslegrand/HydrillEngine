#pragma once

#include <BaseApp.hpp>

/**
* @brief Editor application
*/
class GameApp : public BaseApp
{

//	Constructors & Destructors

public:

	GameApp();
	virtual ~GameApp();

//	Functions

private:

	/**
	* @brief Render loop without engine update, used by windows callbacks
	*/
	void ResizeUpdate() override;

public:
	/**
	@brief Close the application
	*/
	void CloseApp();

	/**
	* @brief Initialize the editor application
	* @param windowName : name of the window
	*/
	void Initialize(const char* windowName = "HydrillGame") override;

	void FrameStart() override;

	void FrameEnd() override;

	void Update() override;

	void Render() override;

	void UpdateImageDimensions();

	/**
	* @brief Main loop, it contains the update and rendering phases
	*/
	void Loop() override;

	/**
	* @brief Shutdown the application
	*/
	void Shutdown() override;
};